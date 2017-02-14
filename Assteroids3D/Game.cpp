#include "Game.h"

#include <iostream>

struct MinFilter {
	GLint           mode;
	const char*     modeStr;
	float           anisotropy;
	const char*     descr;
};

struct MagFilter {
	GLint           mode;
	const char*     modeStr;
	const char*     descr;
};

const MinFilter g_minFilters[] = {
	{ GL_NEAREST,               "GL_NEAREST",               1.0f,   "Nearest, no mipmaps" },
	{ GL_LINEAR,                "GL_LINEAR",                1.0f,   "Bilinear, no mipmaps" },
	{ GL_LINEAR_MIPMAP_NEAREST, "GL_LINEAR_MIPMAP_NEAREST", 1.0f,   "Bilinear, nearest mipmap" },
	{ GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  1.0f,   "Trilinear" },
	{ GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  2.0f,   "Anisotropic x2" },
	{ GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  4.0f,   "Anisotropic x4" },
	{ GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  8.0f,   "Anisotropic x8" },
	{ GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR", 16.0f,   "Anisotropic x16" },
};
const int g_numMinFilters = sizeof(g_minFilters) / sizeof(g_minFilters[0]);

const MagFilter g_magFilters[] = {
	{ GL_NEAREST,   "GL_NEAREST",   "Nearest" },
	{ GL_LINEAR,    "GL_LINEAR",    "Linear" },
};
const int g_numMagFilters = sizeof(g_magFilters) / sizeof(g_magFilters[0]);

Game::Game()
{
}

Game::~Game()
{
}

bool Game::initialize(int w, int h)
{

	currentState = PAUSED;

	timeSinceLastFire = fireRate;

	glEnable(GL_DEPTH_TEST);    // !!!!!!111!!1!!!11!^&#(!@^(!!!!!!

	glEnable(GL_CULL_FACE);

	// build shaders
	uColorProg = BuildShaderProgram("shaders/ucolor-vs.glsl", "shaders/ucolor-fs.glsl");
	dirLightProg = BuildShaderProgram("shaders/ucolor-DirLight-vs.glsl", "shaders/ucolor-DirLight-fs.glsl");
	effectsProg = glsh::BuildShaderProgram("shaders/TexNoLight-vs.glsl", "shaders/TexNoLight-fs.glsl");
	texTintProgram = glsh::BuildShaderProgram("shaders/TexNoLight-vs.glsl", "shaders/TexTintNoLight-fs.glsl");

	// window aspect ratio
	float aspectRatio = w / (float)h;

	// set background color (yay cornflower blue)
	glClearColor(0.01f, 0.03f, 0.06f, 1.0f);

	shipMesh = (glsh::IndexedMesh*)LoadWavefrontOBJ("meshes/player-ship.obj");
	asteroidMesh = (glsh::IndexedMesh*)LoadWavefrontOBJ("meshes/asteroid.obj");
	missileMesh = (glsh::IndexedMesh*)LoadWavefrontOBJ("meshes/missile.obj");
	enemyShipMesh = (glsh::IndexedMesh*)LoadWavefrontOBJ("meshes/enemy-ship.obj");
	enemyMissileMesh = (glsh::IndexedMesh*)LoadWavefrontOBJ("meshes/enemy-missile.obj");

	InitGame();

	explosionSheet = TextureSheet::Create("media/explosion.tga", 16);
	if (!explosionSheet) {
		std::cout << "failed to load explosion" << std::endl;
	}

	blendMode = kAdditiveBlending;

	font = glsh::CreateFont("fonts/Consolas13");

	mTexMgr = new TextureManager("textures/");

	InitTextures();

	UpdateScorePanel();
	UpdateLivesPanel();

	// set UI TextBatches
	SetUIText();

	// lights
	LightCol = glm::vec3(0.8f, 0.8f, 0.75f);
	AmbientCol = glm::vec3(0, 0, 0.1f);

	// initialize camera
	mainCamera = new glsh::FreeLookCamera(this);
	mainCamera->setPosition(0, 0.0f, 100.0f);
	mainCamera->lookAt(0, 0.0f, -100.0f);
	mainCamera->mFOV = w / h * 10.0f;

	updateProjection();

	return true;
}

void Game::updateProjection()
{
	// get window dimensions
	int w = getWindow()->getWidth();
	int h = getWindow()->getHeight();

	// window aspect ratio
	float aspectRatio = w / (float)h;

	// dimensions of viewable area
	mViewHeight = mainCamera->mFOV;
	mViewWidth = mViewHeight * aspectRatio;

	// bounds of viewable area
	mViewLeft = -0.5f * mViewWidth;
	mViewRight = mViewLeft + mViewWidth;
	mViewBottom = -0.5f * mViewHeight;
	mViewTop = mViewBottom + mViewHeight;
}

void Game::shutdown()
{
	// cleanup
	for (auto & a : asteroids)
	{
		delete a;
	}
	for (auto & m : missiles)
	{
		delete m;
	}
	for (auto & m : enemyMissiles)
	{
		delete m;
	}

	delete asteroidMesh;
	delete missileMesh;
	delete shipMesh;
}

void Game::InitTextures()
{
	//
	// Create texture to render Game2 into and attach it to a Framebuffer Object
	//
	{
		mFBOWidth = 256;
		mFBOHeight = 256;

		// create texture
		glGenTextures(1, &mFBOTex);
		glBindTexture(GL_TEXTURE_2D, mFBOTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mFBOWidth, mFBOHeight,
			0, GL_RGB, GL_UNSIGNED_BYTE, NULL);            // allocate texture without sending any data

														   // only one valid mipmap level for now
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		// create framebuffer object
		glGenFramebuffers(1, &mFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFBOTex, 0);  // set framebuffer texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // unbind for now

		mMeshTextures.addItem(mFBOTex);
	}
}

void Game::resize(int w, int h)
{
	// update OpenGL viewport
	glViewport(0, 0, w, h);

	mScrWidth = (float)w;
	mScrHeight = (float)h;
	mScrTop = (float)(h - 1);

	updateProjection();
}

void Game::draw()
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentState == PLAYING)
	{

		DrawMissiles();
		DrawAsteroids();
		DrawPlayer();
		if (enemyShip != nullptr)
		{
			DrawEnemyShip();
		}

		// render explosion effects
		glUseProgram(effectsProg);

		if (blendMode == kDisableBlending) {
			glDisable(GL_BLEND);
		}
		else {
			glEnable(GL_BLEND);
			if (blendMode == kAdditiveBlending) {
				// additive blending function
				glBlendFunc(GL_ONE, GL_ONE);
				glsh::SetShaderUniform("u_BlendWeight", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));  // fragment color multiplier for additive blending
			}
			else {
				// alpha blending function
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glsh::SetShaderUniform("u_BlendWeight", glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));  // fragment color multiplier for alpha blending
			}
		}

		glBlendFunc(GL_ONE, GL_ONE);
		glsh::SetShaderUniform("u_BlendWeight", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		glsh::SetShaderUniform("u_ProjectionMatrix", glm::ortho(mViewLeft, mViewRight, mViewBottom, mViewTop));
		glsh::SetShaderUniform("u_TexSampler", 0);
		for (auto & effect : effectlist) {
			if (effect != NULL) {
				glm::mat4 tf = glsh::CreateTranslation(effect->mPos.x, effect->mPos.y, 0);
				glm::mat4 rm = glsh::CreateRotationZ(effect->mAngle);
				glsh::SetShaderUniform("u_ModelviewMatrix", tf* rm);
				effect->DrawCurrentFrame();
			}
		}
		// disable for UI drawing
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		//glDisable(GL_MULTISAMPLE);

		// draw score panel
		glm::vec4 textColor(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 bgColor(0.1f, 0.1f, 0.1f, 0.3f);
		glm::vec4 borderColor(1.0f, 1.0f, 1.0f, 0.25f);
		float margin = 10;

		glm::vec2 scoreOffset = glm::vec2(20.0f, -20.0f);
		glm::vec2 livesOffset = glm::vec2(20.0f, -60.0f);

		DrawTextArea(scoreTextBatch, glm::vec2(scoreOffset.x, scoreOffset.y + mScrTop), margin, textColor, bgColor, borderColor);
		DrawTextArea(livesTextBatch, glm::vec2(livesOffset.x, livesOffset.y + mScrTop), margin, textColor, bgColor, borderColor);
	}
	// draw menu
	else if (currentState == PAUSED)
	{
		// disable for UI drawing
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glm::vec4 textColor(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 bgColor(0.1f, 0.1f, 0.1f, 0.3f);
		glm::vec4 borderColor(1.0f, 1.0f, 1.0f, 0.25f);

		DrawTextArea(newGameTextBatch, glm::vec2(NEW_GAME_RECT.x - newGameTextBatch.GetWidth() * 0.5f, mScrTop - NEW_GAME_RECT.z), BUTTON_MARGIN, textColor, bgColor, borderColor);
		DrawTextArea(quitTextBatch, glm::vec2(QUIT_RECT.x - quitTextBatch.GetWidth() * 0.5f, mScrTop - QUIT_RECT.z), BUTTON_MARGIN, textColor, bgColor, borderColor);
	}
	else if (currentState == GAME_OVER)
	{
		// disable for UI drawing
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glm::vec4 textColor(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 bgColor(0.1f, 0.1f, 0.1f, 0.3f);
		glm::vec4 borderColor(1.0f, 1.0f, 1.0f, 0.25f);

		glm::vec2 scoreOffset = glm::vec2(350.0f, -140.0f);

		DrawTextArea(newGameTextBatch, glm::vec2(NEW_GAME_RECT.x - newGameTextBatch.GetWidth() * 0.5f, mScrTop - NEW_GAME_RECT.z), BUTTON_MARGIN, textColor, bgColor, borderColor);
		DrawTextArea(scoreTextBatch, glm::vec2(scoreOffset.x, scoreOffset.y + mScrTop), BUTTON_MARGIN, textColor, bgColor, borderColor);
		DrawTextArea(quitTextBatch, glm::vec2(QUIT_RECT.x - quitTextBatch.GetWidth() * 0.5f, mScrTop - QUIT_RECT.z), BUTTON_MARGIN, textColor, bgColor, borderColor);
	}
}

void Game::update(float dt)
{
	// get pointer to keyboard data
	const glsh::Keyboard* kb = getKeyboard();

	// quit if Escape key was pressed
	if (kb->keyPressed(glsh::KC_ESCAPE)) {
		//quit();
		if (currentState == PAUSED || currentState == GAME_OVER)
		{
			currentState = PLAYING;
		}
		else if (currentState == PLAYING)
		{
			currentState = PAUSED;
		}
	}

	if (currentState == PAUSED || currentState == GAME_OVER)
	{
		// handle mouse click
		const glsh::Mouse* mouse = getMouse();
		if (mouse->buttonPressed(glsh::MOUSE_BUTTON_LEFT)) {
			int scrX = mouse->getX();
			int scrY = mouse->getY();

			// check for new game click
			if (PointInRect(glm::vec2(scrX, scrY),
				glm::vec4(NEW_GAME_RECT.x - newGameTextBatch.GetWidth() * 0.5f,
					NEW_GAME_RECT.x - (newGameTextBatch.GetWidth() * 0.5f) + newGameTextBatch.GetWidth() + BUTTON_MARGIN * 2,
					NEW_GAME_RECT.z,
					NEW_GAME_RECT.z + newGameTextBatch.GetHeight() + BUTTON_MARGIN * 2)))
			{
				CleanUpGame();
				InitGame();
				currentState = PLAYING;
			}

			// check for quit click
			if (PointInRect(glm::vec2(scrX, scrY),
				glm::vec4(QUIT_RECT.x - quitTextBatch.GetWidth() * 0.5f,
					QUIT_RECT.x - (quitTextBatch.GetWidth() * 0.5f) + quitTextBatch.GetWidth() + BUTTON_MARGIN * 2,
					QUIT_RECT.z,
					QUIT_RECT.z + quitTextBatch.GetHeight() + BUTTON_MARGIN * 2)))
			{
				quit();
			}

		}
	}
	// game not paused
	else
	{
		timeSinceLastFire += dt;
		lastEnemySpawn += dt;

		// turn right
		if (kb->isKeyDown(glsh::KC_RIGHT))
		{
			playerShip->UpdateYaw(-2.0f);
		}
		// turn left
		if (kb->isKeyDown(glsh::KC_LEFT))
		{
			playerShip->UpdateYaw(2.0f);
		}
		// move forward
		if (kb->isKeyDown(glsh::KC_UP))
		{
			playerShip->SetSpeed(2.5f);
		}
		else if (kb->isKeyDown(glsh::KC_DOWN))
		{
			playerShip->SetSpeed(-1.5f);
		}
		else
		{
			playerShip->SetSpeed(0.0f);
		}

		// spawn enemy?
		if (lastEnemySpawn > enemyInterval)
		{
			lastEnemySpawn = 0.0f;
			if (enemyShip != nullptr)
			{
				delete enemyShip;
				enemyShip = nullptr;
			}
			enemyShip = new EnemyShip();
			enemyShip->SetMesh(enemyShipMesh);
			enemyShip->SetPosition(glm::vec3(-6.9f, 2.0f, 0.0f));
			enemyShip->SetYaw(glm::radians(0.0f));
			enemyShip->SetSpeed(3.0f);
			enemyShip->SetScale(glm::vec3(0.3f));
			enemyShip->Initialize();
		}

		// fire/spawn missile
		if (kb->isKeyDown(glsh::KC_SPACE))
		{
			if (timeSinceLastFire >= fireRate)
			{
				Missile* m = new Missile();
				m->SetMesh(missileMesh);
				m->SetPosition(playerShip->GetPosition());
				m->SetYaw(playerShip->GetYaw());
				m->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
				m->SetSpeed(5.0f);
				m->SetYaw(playerShip->GetYaw());
				m->Initialize();
				missiles.push_back(m);
				timeSinceLastFire = 0.0f;
			}
		}

		// re-populate with asteroids if need be
		if (asteroids.size() <= 0)
		{
			// reset player position
			playerShip->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			// initialize asteroids
			for (int i = 0; i < 3; i++)
			{
				float radius = 5.0f;
				float angle = glsh::Random(0.0f, 360.f);
				SpawnAsteroid(glm::vec3(radius * cos(angle), radius * sin(angle), 0.0f), ASTEROID_SCALE);
			}
		}

		playerShip->Update(dt, mainCamera->mFOV, getWindow()->getWidth(), getWindow()->getHeight());
		// update asteroid list
		for (auto & a : asteroids)
		{
			float scaler = 0.6f;
			if (a->dead)
			{
				if (a->GetScale().x > ASTEROID_SCALE * scaler * scaler)
				{
					for (int i = 0; i < 3; i++)
					{
						SpawnAsteroid(a->GetPosition(), a->GetScale().x * scaler);
					}
				}
				currentScore += 10;
				UpdateScorePanel();
				delete a;
				asteroids.remove(a);
				break;
			}
			else
			{
				a->Update(dt, mainCamera->mFOV, getWindow()->getWidth(), getWindow()->getHeight());
			}

		}
		// update missile list
		for (auto & m : missiles)
		{
			if (m->dead)
			{
				effectlist.push_back(new AnimatedEffect(explosionSheet, 1.0f,
					glm::vec2(m->GetPosition().x - (m->GetScale().x * 0.5f), m->GetPosition().y - (m->GetScale().y * 0.5f)),
					m->GetPitch()));
				delete m;
				missiles.remove(m);
				break;
			}
			else
			{
				if (m->lifetime <= 0.0f)
				{
					delete m;
					missiles.remove(m);
					break;
				}
				m->lifetime -= dt;
				m->Update(dt, mainCamera->mFOV, getWindow()->getWidth(), getWindow()->getHeight());
			}
		}
		// update enemy missile list
		// update missile list
		for (auto & m : enemyMissiles)
		{
			if (m->dead)
			{
				effectlist.push_back(new AnimatedEffect(explosionSheet, 1.0f,
					glm::vec2(m->GetPosition().x - (m->GetScale().x * 0.5f), m->GetPosition().y - (m->GetScale().y * 0.5f)),
					m->GetPitch()));
				delete m;
				enemyMissiles.remove(m);
				break;
			}
			else
			{
				if (m->lifetime <= 0.0f)
				{
					delete m;
					enemyMissiles.remove(m);
					break;
				}
				m->lifetime -= dt;
				m->Update(dt, mainCamera->mFOV, getWindow()->getWidth(), getWindow()->getHeight());
			}
		}

		if (enemyShip != nullptr)
		{
			if (enemyShip->dead)
			{
				// do stuff
				delete enemyShip;
				enemyShip = nullptr;
			}
			else
			{
				glm::vec2 displacement = glm::vec2(playerShip->GetPosition().x - enemyShip->GetPosition().x, (playerShip->GetPosition().y - enemyShip->GetPosition().y));
				enemyShip->SetYaw(glm::degrees(atan2(displacement.y, displacement.x)));
				enemyShip->Update(dt, mainCamera->mFOV, getWindow()->getWidth(), getWindow()->getHeight());
				// fire?
				if (enemyShip->Fire())
				{
					Missile* m = new Missile();
					m->SetMesh(enemyMissileMesh);
					m->SetPosition(enemyShip->GetPosition());
					m->SetYaw(enemyShip->GetYaw());
					m->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
					m->SetSpeed(5.0f);
					m->SetYaw(enemyShip->GetYaw());
					m->Initialize();
					enemyMissiles.push_back(m);
				}

			}
		}

		for (auto & m : missiles)
		{
			for (auto & a : asteroids)
			{
				if (m->CheckCollision(a))
				{
					m->dead = true;
					a->dead = true;
				}
			}

			if (enemyShip != nullptr && m->CheckCollision(enemyShip))
			{
				currentScore += 100;
				UpdateScorePanel();
				m->dead = true;
				enemyShip->dead = true;
			}
		}

		for (auto & a : asteroids)
		{
			if (!a->dead && a->CheckCollision(playerShip))
			{
				// player death
				effectlist.push_back(new AnimatedEffect(explosionSheet, 1.0f,
					glm::vec2(playerShip->GetPosition().x - (playerShip->GetScale().x * 0.5f), playerShip->GetPosition().y - (playerShip->GetScale().y * 0.5f)),
					playerShip->GetPitch()));
				currentLives -= 1;
				UpdateLivesPanel();
				// delete and rebuild player
				if (enemyShip != nullptr)
				{
					delete enemyShip;
					enemyShip = nullptr;
				}
				delete playerShip;
				ResetGame();
				break;
			}
		}

		for (auto & m : enemyMissiles)
		{
			if (!m->dead && m->CheckCollision(playerShip))
			{
				// player death
				effectlist.push_back(new AnimatedEffect(explosionSheet, 1.0f,
					glm::vec2(playerShip->GetPosition().x - (playerShip->GetScale().x * 0.5f), playerShip->GetPosition().y - (playerShip->GetScale().y * 0.5f)),
					playerShip->GetPitch()));
				currentLives -= 1;
				UpdateLivesPanel();
				// delete and rebuild player
				if (enemyShip != nullptr)
				{
					delete enemyShip;
					enemyShip = nullptr;
				}
				delete playerShip;
				ResetGame();
				break;
			}
		}

		for (auto effect : effectlist) {
			effect->AddTime(dt);
			if (effect->FinishedPlaying()) {
				effectlist.remove(effect);
				break;
			}
		}

		mainCamera->update(dt);
	}

}

void Game::InitGame()
{
	timeSinceLastFire = 0.0f;
	lastEnemySpawn = 0.0f;

	currentScore = 0;
	currentLives = 3;
	UpdateScorePanel();
	UpdateLivesPanel();

	asteroids = std::list<Asteroid*>();
	missiles = std::list<Missile*>();
	enemyMissiles = std::list<Missile*>();
	effectlist = std::list<AnimatedEffect*>();

	playerShip = new Ship();
	playerShip->SetMesh(shipMesh);
	playerShip->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	playerShip->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
	playerShip->Initialize();

	// initialize asteroids
	for (int i = 0; i < 3; i++)
	{
		float radius = 5.0f;
		float angle = glsh::Random(0.0f, 360.f);
		SpawnAsteroid(glm::vec3(radius * cos(angle), radius * sin(angle), 0.0f), ASTEROID_SCALE);
	}
}

void Game::ResetGame()
{
	if (currentLives <= 0)
	{
		// game over
		currentState = GAME_OVER;
	}
	else
	{
		timeSinceLastFire = 0.0f;
		lastEnemySpawn = 0.0f;

		asteroids = std::list<Asteroid*>();
		missiles = std::list<Missile*>();
		enemyMissiles = std::list<Missile*>();
		effectlist = std::list<AnimatedEffect*>();

		if (enemyShip != nullptr)
		{
			delete enemyShip;
			enemyShip == nullptr;
		}

		playerShip = new Ship();
		playerShip->SetMesh(shipMesh);
		playerShip->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		playerShip->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		playerShip->Initialize();

		// initialize asteroids
		for (int i = 0; i < 3; i++)
		{
			float radius = 5.0f;
			float angle = glsh::Random(0.0f, 360.f);
			SpawnAsteroid(glm::vec3(radius * cos(angle), radius * sin(angle), 0.0f), ASTEROID_SCALE);
		}

		currentState = PLAYING;
	}

}

void Game::CleanUpGame()
{
	// cleanup
	for (auto & a : asteroids)
	{
		delete a;
	}
	for (auto & m : missiles)
	{
		delete m;
	}
	for (auto & e : effectlist)
	{
		delete e;
	}
	if (enemyShip != nullptr)
	{
		delete enemyShip;
		enemyShip == nullptr;
	}
	//if (playerShip != nullptr)
	//{
	//	delete playerShip;
	//}
}

GLuint Game::BuildShaderProgram(std::string vertexPath, std::string fragmentPath)
{
	// load and compile the vertex shader

	// load shader source code from a text file
	std::string vsString = glsh::ReadTextFile(vertexPath);

	// create vertex shader object
	GLuint vso = glCreateShader(GL_VERTEX_SHADER);
	if (!vso) {
		std::cerr << "*** Poop: Failed to create shader object" << std::endl;
		return false;
	}

	// set shader source code
	const char* vsSource[1];
	vsSource[0] = vsString.c_str();
	glShaderSource(vso, 1, vsSource, NULL);

	// compile the shader
	glCompileShader(vso);

	// load and compile the fragment shader

	// load shader source code from a text file
	std::string fsString = glsh::ReadTextFile(fragmentPath);

	// create fragment shader object
	GLuint fso = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fso) {
		std::cerr << "*** Poop: Failed to create shader object" << std::endl;
		return false;
	}

	// set shader source code
	const char* fsSource[1];
	fsSource[0] = fsString.c_str();
	glShaderSource(fso, 1, fsSource, NULL);

	// compile the shader
	glCompileShader(fso);

	// create and link shader program

	// create program object
	GLuint prog = glCreateProgram();
	if (!prog) {
		std::cerr << "*** Poop: Failed to create program object" << std::endl;
		return false;
	}

	// attach vertex and fragment shaders to the program
	glAttachShader(prog, vso);
	glAttachShader(prog, fso);

	// link the program
	glLinkProgram(prog);

	// TODO: check link status (omitted for brevity)

	// mark shader objects ready for deletion
	glDeleteShader(vso);
	glDeleteShader(fso);

	return prog;
}

void Game::DrawAsteroids()
{
	// create projection matrix
	glm::mat4 projMatrix = mainCamera->getProjectionMatrix();
	glm::mat4 viewMatrix = mainCamera->getViewMatrix();

	glUseProgram(dirLightProg);
	glsh::SetShaderUniform("u_ProjectionMatrix", projMatrix);

	// render asteroid list
	for (auto & a : asteroids)
	{
		viewMatrix = mainCamera->getViewMatrix();

		// set lighting parameters for the directional light shader
		glm::vec3 lightDir(1.5f, 2.0f, 3.0f);           // direction to light in world space
		lightDir = glm::mat3(viewMatrix) * lightDir;    // direction to light in camera space
		lightDir = glm::normalize(lightDir);            // normalized for sanity
		glsh::SetShaderUniform("u_LightDir", lightDir);
		glsh::SetShaderUniform("u_LightColor", LightCol);
		glsh::SetShaderUniform("u_AmbientCol", AmbientCol);

		viewMatrix = glm::translate(viewMatrix, a->GetPosition());
		viewMatrix = glm::rotate(viewMatrix, glm::radians(a->GetRoll()), glm::vec3(0.0f, 0.0f, 1.0f));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(a->GetYaw()), glm::vec3(0.0f, 1.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(a->GetPitch()), glm::vec3(1.0f, 0.0f, 0.0f));
		viewMatrix = glm::scale(viewMatrix, a->GetScale());

		glsh::SetShaderUniform("u_ModelViewMatrix", viewMatrix);

		// set transform and material properties
		glsh::SetShaderUniform("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(viewMatrix))));

		// set material properties
		glsh::SetShaderUniform("u_Color", glm::vec4(0.545f, 0.27f, 0.07f, 1.0f));

		a->Render();
	}
}

void Game::DrawEnemyShip()
{
	if (enemyShip != nullptr)
	{
		// create projection matrix
		glm::mat4 projMatrix = mainCamera->getProjectionMatrix();
		glm::mat4 viewMatrix = mainCamera->getViewMatrix();

		glUseProgram(dirLightProg);
		glsh::SetShaderUniform("u_ProjectionMatrix", projMatrix);

		// draw ship
		viewMatrix = mainCamera->getViewMatrix();

		// set lighting parameters for the directional light shader
		glm::vec3 lightDir(1.5f, 2.0f, 3.0f);           // direction to light in world space
		lightDir = glm::mat3(viewMatrix) * lightDir;    // direction to light in camera space
		lightDir = glm::normalize(lightDir);            // normalized for sanity
		glsh::SetShaderUniform("u_LightDir", lightDir);
		glsh::SetShaderUniform("u_LightColor", LightCol);
		glsh::SetShaderUniform("u_AmbientCol", AmbientCol);

		viewMatrix = glm::translate(viewMatrix, enemyShip->GetPosition());
		viewMatrix = glm::rotate(viewMatrix, glm::radians(enemyShip->GetYaw()), glm::vec3(0.0f, 0.0f, 1.0f));
		viewMatrix = glm::scale(viewMatrix, enemyShip->GetScale());

		glsh::SetShaderUniform("u_ModelViewMatrix", viewMatrix);

		// set transform and material properties
		glsh::SetShaderUniform("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(viewMatrix))));

		// set material properties
		glsh::SetShaderUniform("u_Color", glm::vec4(0.8f, 0.1f, 0.05f, 1.0f));

		if (enemyShip == nullptr)
		{
			std::cout << "IT FUCKING HAPPENED" << std::endl;
		}

		enemyShip->Render();
	}

}

void Game::DrawMissiles()
{
	// create projection matrix
	glm::mat4 projMatrix = mainCamera->getProjectionMatrix();
	glm::mat4 viewMatrix = mainCamera->getViewMatrix();

	glUseProgram(dirLightProg);
	glsh::SetShaderUniform("u_ProjectionMatrix", projMatrix);

	// render missile list
	for (auto & m : missiles)
	{
		viewMatrix = mainCamera->getViewMatrix();

		// set lighting parameters for the directional light shader
		glm::vec3 lightDir(1.5f, 2.0f, 3.0f);           // direction to light in world space
		lightDir = glm::mat3(viewMatrix) * lightDir;    // direction to light in camera space
		lightDir = glm::normalize(lightDir);            // normalized for sanity
		glsh::SetShaderUniform("u_LightDir", lightDir);
		glsh::SetShaderUniform("u_LightColor", LightCol);
		glsh::SetShaderUniform("u_AmbientCol", AmbientCol);

		viewMatrix = glm::translate(viewMatrix, m->GetPosition());
		//viewMatrix *= m->GetRotationMatrix();
		viewMatrix = glm::rotate(viewMatrix, glm::radians(m->GetYaw()), glm::vec3(0.0f, 0.0f, 1.0f));
		viewMatrix = glm::scale(viewMatrix, m->GetScale());

		glsh::SetShaderUniform("u_ModelViewMatrix", viewMatrix);

		// set transform and material properties
		glsh::SetShaderUniform("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(viewMatrix))));

		// set material properties
		glsh::SetShaderUniform("u_Color", glm::vec4(0.0f, 0.4f, 0.8f, 1.0f));

		m->Render();
	}

	// render missile list
	for (auto & m : enemyMissiles)
	{
		viewMatrix = mainCamera->getViewMatrix();

		// set lighting parameters for the directional light shader
		glm::vec3 lightDir(1.5f, 2.0f, 3.0f);           // direction to light in world space
		lightDir = glm::mat3(viewMatrix) * lightDir;    // direction to light in camera space
		lightDir = glm::normalize(lightDir);            // normalized for sanity
		glsh::SetShaderUniform("u_LightDir", lightDir);
		glsh::SetShaderUniform("u_LightColor", LightCol);
		glsh::SetShaderUniform("u_AmbientCol", AmbientCol);

		viewMatrix = glm::translate(viewMatrix, m->GetPosition());
		//viewMatrix *= m->GetRotationMatrix();
		viewMatrix = glm::rotate(viewMatrix, glm::radians(m->GetYaw()), glm::vec3(0.0f, 0.0f, 1.0f));
		viewMatrix = glm::scale(viewMatrix, m->GetScale());

		glsh::SetShaderUniform("u_ModelViewMatrix", viewMatrix);

		// set transform and material properties
		glsh::SetShaderUniform("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(viewMatrix))));

		// set material properties
		glsh::SetShaderUniform("u_Color", glm::vec4(0.8f, 0.8f, 0.1f, 1.0f));

		m->Render();
	}
}

void Game::DrawPlayer()
{
	// create projection matrix
	glm::mat4 projMatrix = mainCamera->getProjectionMatrix();
	glm::mat4 viewMatrix = mainCamera->getViewMatrix();

	glUseProgram(dirLightProg);
	glsh::SetShaderUniform("u_ProjectionMatrix", projMatrix);

	// draw ship
	viewMatrix = mainCamera->getViewMatrix();

	// set lighting parameters for the directional light shader
	glm::vec3 lightDir(1.5f, 2.0f, 3.0f);           // direction to light in world space
	lightDir = glm::mat3(viewMatrix) * lightDir;    // direction to light in camera space
	lightDir = glm::normalize(lightDir);            // normalized for sanity
	glsh::SetShaderUniform("u_LightDir", lightDir);
	glsh::SetShaderUniform("u_LightColor", LightCol);
	glsh::SetShaderUniform("u_AmbientCol", AmbientCol);

	viewMatrix = glm::translate(viewMatrix, playerShip->GetPosition());
	viewMatrix = glm::rotate(viewMatrix, glm::radians(playerShip->GetYaw()), glm::vec3(0.0f, 0.0f, 1.0f));
	viewMatrix = glm::scale(viewMatrix, playerShip->GetScale());

	glsh::SetShaderUniform("u_ModelViewMatrix", viewMatrix);

	// set transform and material properties
	glsh::SetShaderUniform("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(viewMatrix))));

	// set material properties
	glsh::SetShaderUniform("u_Color", glm::vec4(0.0f, 0.8f, 0.4f, 1.0f));

	playerShip->Render();
}

void Game::DrawTextArea(const glsh::TextBatch& textBatch, const glm::vec2& pos, float margin, const glm::vec4& textColor, const glm::vec4& bgColor, const glm::vec4& borderColor)
{
	float bgWidth = textBatch.GetWidth() + 2 * margin;
	float bgHeight = textBatch.GetHeight() + 2 * margin;

	// background fill quad (triangle strip)
	glsh::VertexPosition bgQuad[] = {
		glsh::VertexPosition(0,       -bgHeight, 0),    // bottom-left
		glsh::VertexPosition(bgWidth, -bgHeight, 0),    // bottom-right
		glsh::VertexPosition(0,         0, 0),    // top-left
		glsh::VertexPosition(bgWidth,         0, 0),    // top-right
	};

	// background border (line loop)
	glsh::VertexPosition border[] = {
		glsh::VertexPosition(0,       -bgHeight, 0),    // bottom-left
		glsh::VertexPosition(bgWidth, -bgHeight, 0),    // bottom-right
		glsh::VertexPosition(bgWidth,         0, 0),    // top-right
		glsh::VertexPosition(0,         0, 0),    // top-left
	};

	glm::mat4 uiProj = glm::ortho(-0.5f, mScrWidth - 0.5f, -0.5f, mScrHeight - 0.5f, -1.0f, 1.0f);

	glUseProgram(uColorProg);

	// draw background fill
	glsh::SetShaderUniform("u_ProjectionMatrix", uiProj);
	glsh::SetShaderUniform("u_ModelViewMatrix", glsh::CreateTranslation(pos.x, pos.y, 0.0f));
	glsh::SetShaderUniform("u_Color", bgColor);
	glsh::DrawGeometry(GL_TRIANGLE_STRIP, bgQuad, 4);

	// draw background border
	glsh::SetShaderUniform("u_Color", borderColor);
	glsh::DrawGeometry(GL_LINE_LOOP, border, 4);

	// draw text
	glUseProgram(texTintProgram);
	glsh::SetShaderUniform("u_ProjectionMatrix", uiProj);
	glsh::SetShaderUniform("u_ModelviewMatrix", glsh::CreateTranslation(pos.x + margin, pos.y - margin, 0.0f));
	glsh::SetShaderUniform("u_Tint", textColor);

	glBindTexture(GL_TEXTURE_2D, textBatch.GetFont()->getTex());

	textBatch.DrawGeometry();
}

void Game::ApplyFilteringSettings(GLuint sampler)
{
	// get current settings
	const MinFilter& minFilter = g_minFilters[0];
	const MagFilter& magFilter = g_magFilters[0];

	// configure the sampler
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, magFilter.mode);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, minFilter.mode);
	glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, minFilter.anisotropy);
}

void Game::UpdateScorePanel()
{
	std::ostringstream ss;

	ss << "Score: " << currentScore;

	scoreTextBatch.SetText(font, ss.str(), false);
}

void Game::UpdateLivesPanel()
{
	std::ostringstream ss;

	ss << "Lives: " << currentLives;

	livesTextBatch.SetText(font, ss.str(), false);
}

void Game::SetUIText() {
	std::ostringstream ss;

	ss << "New Game";
	newGameTextBatch.SetText(font, ss.str(), true);

	ss.str("");
	ss.clear();
	ss << "Restart Game";
	restartTextBatch.SetText(font, ss.str(), true);

	ss.str("");
	ss.clear();
	ss << "Quit Game";
	quitTextBatch.SetText(font, ss.str(), true);
}

bool Game::PointInRect(glm::vec2 pos, glm::vec4 rect)
{
	if (pos.x >= rect.x && pos.x <= rect.y
		&& pos.y >= rect.z && pos.y <= rect.w)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Game::SpawnAsteroid(glm::vec3 position, float scale)
{
	Asteroid* a = new Asteroid();
	a->SetMesh(asteroidMesh);
	a->SetPosition(position);
	// set starting rotations
	a->SetYaw(glsh::Random(0.0f, 360.f));
	a->SetRoll(glsh::Random(0.0f, 360.f));
	a->SetPitch(glsh::Random(0.0f, 360.f));
	// set rotation speeds
	int sign = glsh::Random(-1.0f, 1.0f);
	if (sign >= 0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}
	a->SetYawRotationSpeed(15.0f * (float)sign);
	sign = glsh::Random(-1.0f, 1.0f);
	if (sign >= 0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}
	a->SetPitchRotationSpeed(15.0f * (float)sign);
	sign = glsh::Random(-1.0f, 1.0f);
	if (sign >= 0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}
	a->SetRollRotationSpeed(15.0f * (float)sign);
	// set scale
	a->SetScale(glm::vec3(scale, scale, scale));
	// set speed to init velocity
	a->SetSpeed(2.0f);
	// call initialize
	a->Initialize();
	// add to list
	asteroids.push_back(a);
}