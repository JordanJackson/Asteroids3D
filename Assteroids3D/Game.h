#ifndef GAME_H_
#define GAME_H_

#include "Asteroid.h"
#include "CircularListSelector.h"
#include "EnemyShip.h"
#include "GameObject.h"
#include "GLSH.h"
#include "Missile.h"
#include "Ship.h"
#include "TextureAnimation.h"
#include "TextureManager.h"
#include "Wavefront.h"

const float			ASTEROID_SCALE	=		0.4f;

const glm::vec4		NEW_GAME_RECT	=		glm::vec4(380.0f, 420.0f, 80.0f, 120.0f);
const glm::vec4		QUIT_RECT		=		glm::vec4(380.0f, 420.0f, 200.0f, 220.0f);

const float			BUTTON_MARGIN	=		10.0f;

enum BlendMode {
	kDisableBlending,
	kAlphaBlending,
	kAdditiveBlending,

};

enum GameState {
	PAUSED,
	PLAYING,
	GAME_OVER,
};

class Game : public glsh::App
{
	GLuint					uColorProg = 0;
	GLuint					dirLightProg = 0;
	GLuint					texTintProgram = 0;
	GLuint					effectsProg = 0;

	GLuint					mSampler;

	TextureManager*         mTexMgr;

	glsh::Font*				font;
	glsh::TextBatch			scoreTextBatch;
	glsh::TextBatch			livesTextBatch;
	glsh::TextBatch			newGameTextBatch;
	glsh::TextBatch			restartTextBatch;
	glsh::TextBatch			quitTextBatch;

	GameState				currentState = PAUSED;

    // 2d projection stuff
    float                   mViewWidth                  = 0.0f;
    float                   mViewHeight                 = 0.0f;
    float                   mViewLeft                   = 0.0f;
    float                   mViewRight                  = 0.0f;
    float                   mViewBottom                 = 0.0f;
    float                   mViewTop                    = 0.0f;

	float					mSpinAngle = 0.0f;
	float					timeSinceLastFire;
	float					fireRate = 0.5f;

	float                   mScrWidth, mScrHeight;  // useful for drawing UI stuff
	float                   mScrTop;

	float					enemyInterval = 10.0f;
	float					lastEnemySpawn;
	bool					leftSide = true;

	glsh::FreeLookCamera*	mainCamera;

	TextureSheet*			explosionSheet = nullptr;
	BlendMode				blendMode;
	std::list<AnimatedEffect*> effectlist;

    void                    updateProjection();

	glsh::IndexedMesh*			shipMesh;
	glsh::IndexedMesh*			missileMesh;
	glsh::IndexedMesh*			enemyShipMesh;
	glsh::IndexedMesh*			enemyMissileMesh;
	glsh::IndexedMesh*			asteroidMesh;

	std::list<Asteroid*>	asteroids;
	std::list<Missile*>		missiles;
	std::list<Missile*>     enemyMissiles;
	Ship*					playerShip;
	EnemyShip*				enemyShip;

	CircularListSelector<GLuint>    mMeshTextures;

	GLsizei                 mFBOWidth, mFBOHeight;
	GLuint                  mFBOTex;
	GLuint                  mFBO;

	glm::vec3				LightCol;
	glm::vec3				AmbientCol;

public:
                            Game();
                            ~Game();

    bool                    initialize(int w, int h)    override;
    void                    shutdown()                  override;
    void                    resize(int w, int h)        override;
    void                    draw()                      override;
    void                    update(float dt)            override;

	GLuint					BuildShaderProgram(std::string, std::string);
	void					ApplyFilteringSettings(GLuint sampler);
	void					DrawAsteroids();
	void					DrawEnemyShip();
	void					DrawMissiles();
	void					DrawPlayer();
	void					DrawTextArea(const glsh::TextBatch& textBatch, const glm::vec2& pos, float margin, const glm::vec4& textColor, const glm::vec4& bgColor, const glm::vec4& borderColor);
	void					CleanUpGame();
	void					InitGame();
	void					ResetGame();
	void					InitTextures();
	bool					PointInRect(glm::vec2 pos, glm::vec4 rect);
	void					UpdateScorePanel();
	void					UpdateLivesPanel();
	void					SetUIText();
	void					SpawnAsteroid(glm::vec3 position, float scale);

	int						currentScore = 0;
	int						currentLives = 3;
};

#endif
