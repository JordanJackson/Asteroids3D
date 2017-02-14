#pragma once

#include "GameObject.h"

class Asteroid : public GameObject
{
public:
	Asteroid();
	~Asteroid();

	void Initialize() override;
	void Update(float dt, float fov, int w, int h) override;
};

