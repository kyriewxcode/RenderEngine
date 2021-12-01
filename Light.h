#pragma once
#include "Transform.h"
#include "Math.h"

class Light
{
public:
	Transform transform;
	glm::vec4 color{ 0.12f, 1.0f, 0.92f, 1 };

public:
	Light()
	{
		transform.position = glm::vec3(-2, 2, -2);
	}
};

