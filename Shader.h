#pragma once
#include "Math.h"
#include "Texture.h"
#include "Vertex.h"

class Shader
{
private:
	Texture m_tex;
	glm::mat4 m_matModel;
	glm::mat4 m_matMVP;

public:
	void setTexture(const Texture& tex);

	void setMatModel(const glm::mat4& matModel);

	void setMatMVP(const glm::mat4& matMVP);

	void vertexShader(Vertex& v);

	Color fragmentShader();
};
