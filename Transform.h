#pragma once
#include "Math.h"

class Transform
{
public:
	glm::vec3 position = vecZero;
	glm::vec3 eulerAngles;
	glm::vec3 scale = vecOne;

public:
	Transform() {};
	Transform(glm::vec3 pos, glm::vec3 angle, glm::vec3 scal) : position(pos), eulerAngles(angle), scale(scal) {};

	glm::vec3 right();
	glm::vec3 up();
	glm::vec3 forward();
	glm::quat rotation();

private:
};