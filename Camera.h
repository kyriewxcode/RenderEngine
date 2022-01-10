#pragma once
#ifndef __CAMERA_H_
#define __CAMERA_H_


#include "Math.h"
#include "Transform.h"

class Camera
{
public:
	Transform transform;

public:
	Camera()
	{
		transform.position = glm::vec3(0, 0, -10.f);
	}
	glm::mat4 viewMatrix();
	glm::mat4 projectionMatrix();

private:
	const float m_fov = 45.f;
	const float m_width = WIDTH;
	const float m_height = HEIGHT;
	const float m_zNear = 0.3f;
	const float m_zFar = 1000.f;

private:

};

#endif // !__CAMERA_H_
