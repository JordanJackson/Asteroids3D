#pragma once

#include "GameObject.h"

class Missile : public GameObject
{
public:
	float lifetime;

public:
	Missile();
	~Missile();

	void Initialize();
	void Update(float dt, float fov, int w, int h) override;
};

