#pragma once

#include "GameObject.h"

class Ship : public GameObject
{
public:
	Ship();
	~Ship();

	void Initialize();
	void Update(float dt, float fov, int w, int h) override;
};

