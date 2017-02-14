#include "EnemyShip.h"



EnemyShip::EnemyShip()
{
}


EnemyShip::~EnemyShip()
{
}



void EnemyShip::Initialize()
{
	collider.offset = glm::vec2(0.5f, 0.0f);
	collider.radius = this->scale.x;
}

void EnemyShip::Update(float dt, float fov, int w, int h)
{
	timeSinceLastFire += dt;

	// window aspect ratio
	float aspectRatio = w / (float)h;

	// dimensions of viewable area
	float viewHeight = fov;
	float viewWidth = viewHeight * aspectRatio;

	// bounds of viewable area
	float viewLeft = -0.7f * viewWidth;
	float viewRight = viewLeft + viewWidth * 1.4f;
	float viewBottom = -1.0f * viewHeight;
	float viewTop = viewBottom + viewHeight;

	glm::vec3 tempPosition = position + glm::vec3(velocity.x, velocity.y, 0.0f) * dt;

	// wrap around
	if (tempPosition.x < viewLeft || tempPosition.x > viewRight || tempPosition.y < viewBottom || tempPosition.y > viewHeight)
	{
		dead = true;
	}
	if (!dead)
	{
		UpdatePosition(glm::vec3(velocity.x, velocity.y, 0.0f) * dt, fov, w, h);
		UpdateYaw(yawRotationSpeed * dt);
	}
}

bool EnemyShip::Fire()
{
	if (timeSinceLastFire >= fireRate)
	{
		timeSinceLastFire = 0.0f;
		return true;
	}
	else
	{
		return false;
	}
}