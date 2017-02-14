#include "Missile.h"



Missile::Missile() : lifetime(4.0f)
{
}


Missile::~Missile()
{

}


void Missile::Initialize()
{
	collider.offset = glm::vec2(0.5f, 0.0f);
	collider.radius = this->scale.x;
}

void Missile::Update(float dt, float fov, int w, int h)
{
	UpdatePosition(glm::vec3(velocity.x, velocity.y, 0.0f) * dt, fov, w, h);
}
