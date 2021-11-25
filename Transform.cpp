#include "Transform.h"

glm::vec3 Transform::right()
{
	glm::vec3 right;
	right = glm::normalize(glm::cross(forward(), vecUp));
	return right;
}

glm::vec3 Transform::up()
{
	glm::vec3 up;
	up = glm::normalize(glm::cross(right(), forward()));
	return up;
}

glm::vec3 Transform::forward()
{
	glm::vec3 front;
	front.x = cos(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	front.y = sin(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	front.z = sin(glm::radians(eulerAngles.x));
	return rotation() * vecForward;
}

glm::quat Transform::rotation()
{
	auto quat = glm::quat(glm::vec3(eulerAngles.z, eulerAngles.y, eulerAngles.x));
	return quat;
}

