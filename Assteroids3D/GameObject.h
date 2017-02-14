#pragma once

#include "Collider.h"
#include "GLSH.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <vector>


class GameObject
{
protected:

	glm::vec3						position;
	float							yaw;
	float							pitch;
	float							roll;
	glm::vec3						scale;

	glm::mat4						rotationMatrix;

	float							speed;
	glm::vec2						velocity;
	
	float							yawRotationSpeed;
	float							pitchRotationSpeed;
	float							rollRotationSpeed;

	glsh::Mesh*						mesh;

	Collider						collider;

public:
	bool							dead;

public:
	GameObject() 
		: position(glm::vec3(0.0f, 0.0f, 0.0f)), yaw(0.0f), pitch(0.0f), roll(0.0f), rotationMatrix(glm::mat4(1.0f)), scale(glm::vec3(1.0f, 1.0f, 1.0f)),
		velocity(glm::vec2(0.0f, 0.0f)), yawRotationSpeed(0.0f), pitchRotationSpeed(0.0f), rollRotationSpeed(0.0f), dead(false), mesh(nullptr), collider(Collider())
	{

	}

	virtual ~GameObject()
	{

	}

	Collider GetCollider();
	glm::vec3 GetPosition();
	float GetYaw();
	float GetPitch();
	float GetRoll();
	glm::mat4 GetRotationMatrix();
	glm::vec3 GetScale();
	glm::vec2 GetVelocity();

	virtual void Initialize() = 0;
	virtual void Render();
	virtual void Update(float dt, float fov, int w, int h) = 0;

	void SetMesh(glsh::Mesh* mesh);

	void SetPosition(glm::vec3 pos);
	void SetYaw(float angle);
	void SetPitch(float angle);
	void SetRoll(float angle);

	void SetYawRotationSpeed(float rotationSpeed);
	void SetPitchRotationSpeed(float rotationSpeed);
	void SetRollRotationSpeed(float rotationSpeed);

	void SetScale(glm::vec3 scale);
	void SetSpeed(float speed);
	void SetVelocity(glm::vec2 velocity);

	void UpdatePosition(glm::vec3 pos, float fov, int w, int h);
	void UpdateYaw(float angle);
	void UpdatePitch(float angle);
	void UpdateRoll(float angle);
	void UpdateRotationMatrix();
	void UpdateScale(glm::vec3 scale);
	void UpdateVelocity(glm::vec2 velocity);

	bool CheckCollision(GameObject* other);
	
};

