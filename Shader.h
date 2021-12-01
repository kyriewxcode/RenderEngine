#pragma once
#include "Math.h"
#include "Texture.h"

class Shader
{
private:
	Texture m_tex;
	glm::mat4 m_modelMat;
	glm::mat4 m_mvpMat;

public:
	void setTexture(const Texture& tex);

	void setModelMat(const glm::mat4& modelMat);

	void setMatMVP(const glm::mat4& mvpMat);

	void vertexShader(glm::vec4& v, glm::vec3& n);

	glm::vec4 fragmentShader();
};
