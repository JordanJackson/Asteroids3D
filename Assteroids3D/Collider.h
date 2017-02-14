#pragma once

#include <glm\glm.hpp>

class Collider
{
public:
	glm::vec2		offset;
	float			radius;

public:
	Collider();
	~Collider();
};

