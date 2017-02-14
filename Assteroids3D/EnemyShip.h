#pragma once

#include "GameObject.h"

class EnemyShip : public GameObject
{
private:
	float fireRate = 2.0f;
	float timeSinceLastFire;

public:
	EnemyShip();
	~EnemyShip();

	void Initialize();
	void Update(float dt, float fov, int w, int h) override;

	bool Fire();
};
