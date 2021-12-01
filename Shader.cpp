#include "Shader.h"

void Shader::setTexture(const Texture& tex)
{
	m_tex = tex;
}

void Shader::setModelMat(const glm::mat4& modelMat)
{
	m_modelMat = modelMat;
}

void Shader::setMatMVP(const glm::mat4& mvpMat)
{
	m_mvpMat = mvpMat;
}

void Shader::vertexShader(glm::vec4& v, glm::vec3& n)
{
	v = m_mvpMat * v;
	n = n * glm::mat3x3(glm::inverse(m_modelMat));
}


