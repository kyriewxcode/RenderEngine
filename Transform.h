#pragma once
#include "Math.h"

class Transform
{
public:
	glm::vec3 position = vecZero;
	glm::quat rotation;
	glm::vec3 scale = vecOne;

public:
	Transform() {};
	Transform(glm::vec3 pos, glm::vec3 rotat, glm::vec3 scal) : position(pos), rotation(rotat), scale(scal) {};

	glm::vec3 right() { return rotation * vecRight; }
	glm::vec3 up() { return rotation * vecUp; }
	glm::vec3 forward() { return rotation * vecForward; }
};