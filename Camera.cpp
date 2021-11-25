#include "Camera.h"

glm::mat4 Camera::viewMatrix()
{
	return glm::lookAt(
		transform.position,
		transform.forward(),
		transform.up()
	);
}

glm::mat4 Camera::projectionMatrix()
{
	return glm::perspective(m_fov, m_width / m_height, m_zNear, m_zFar);
}
