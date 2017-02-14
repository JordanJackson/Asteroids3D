#pragma once

class Vertex
{
public:
	float x, y;
	float r, g, b;

	Vertex() : x(0.0f), y(0.0f), r(0.0f), g(0.0f), b(0.0f)
	{

	}

	Vertex(float x, float y, float r, float g, float b) : x(x), y(y), r(r), g(g), b(b)
	{

	}
};