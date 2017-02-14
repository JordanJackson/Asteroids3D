#include "Asteroid.h"

Asteroid::Asteroid()
{
}


Asteroid::~Asteroid()
{
}

void Asteroid::Initialize()
{
	collider.radius = this->scale.x;
}

void Asteroid::Update(float dt, float fov, int w, int h)
{
	UpdatePosition(glm::vec3(velocity.x, velocity.y, 0.0f) * dt, fov, w, h);
	UpdateYaw(yawRotationSpeed * dt);
	UpdatePitch(pitchRotationSpeed * dt);
	UpdateRoll(rollRotationSpeed * dt);
}
