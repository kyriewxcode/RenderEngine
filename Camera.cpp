#include "Camera.h"

glm::mat4 Camera::viewMatrix()
{
	auto view = glm::lookAtRH(
		transform.position,
		transform.forward(),
		transform.up()
	);

	return view;
}

glm::mat4 Camera::projectionMatrix()
{
	return glm::perspective(m_fov, m_width / m_height, m_zNear, m_zFar);
}
