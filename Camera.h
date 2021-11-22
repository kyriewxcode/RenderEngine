#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "mathtool.h"
#include "Entity.h"

struct  Camera
{
	Transform transform;
	const float fov = 45.f;
	const float width = WIDTH;
	const float height = HEIGHT;
	const float zNear = 0.3f;
	const float zFar = 1000.f;

	Camera()
	{
		transform.position = glm::vec3(0, 0, 10.f);
	}
};
#endif // !__CAMERA_H__

