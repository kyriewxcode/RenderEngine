#pragma once
#include "Math.h"
#include "Texture.h"
#include "Geometry.h"

class Shader
{
public:
	void setTexture(const Texture& tex);

	void setModelMat(const glm::mat4& modelMat);

	void setMatMVP(const glm::mat4& mvpMat);

	ShaderContex vertexShader(Vertex& vert);

	void fragmentShader(ShaderContex& frag);


private:
	Texture m_tex;
	glm::mat4 m_modelMat;
	glm::mat4 m_mvpMat;

private:
	glm::vec4 transformClip2NDC(glm::vec4 vert);
	glm::vec3 transformNDC2screen(glm::vec4 vert);
};
