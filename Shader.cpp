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

glm::vec4 Shader::transformClip2NDC(glm::vec4 vert)
{
	glm::vec4 positionCS
	{
		vert.x /= vert.w,
		vert.y /= vert.w,
		vert.z /= vert.w,
		vert.z / vert.w
	};

	return positionCS;
}

glm::vec3 Shader::transformNDC2screen(glm::vec4 vert)
{
	glm::vec3 screenPos;
	float map = (vert.x + 1.f) / 2.f;
	screenPos.x = WIDTH * map;
	map = 1.f - ((vert.y + 1.f) / 2.f);
	screenPos.y = HEIGHT * map;

	float f1 = (50.f - 0.1f) / 2.0f;
	float f2 = (50.f + 0.1f) / 2.0f;
	screenPos.z = vert.z * f1 + f2;
	return screenPos;
}

ShaderContex Shader::vertexShader(Vertex& vert)
{
	ShaderContex output;
	output.worldPos = m_modelMat * vert.pos;
	output.clipPos = m_mvpMat * vert.pos;
	output.screenPos = transformNDC2screen(transformClip2NDC(output.clipPos));
	output.depth = output.screenPos.z;
	output.normal = vert.normal * glm::mat3x3(glm::inverse(m_modelMat));

	return output;
}

void Shader::fragmentShader(ShaderContex& frag)
{
	glm::vec3 lightPos = glm::vec3(2, 2, -9);
	glm::vec3 lightDir = lightPos - frag.worldPos;


	float intensity = glm::dot(lightDir, frag.normal);
	intensity = intensity > 1 ? 1 : intensity;
	intensity = intensity < 0 ? 0 : intensity;
	intensity += 0.2;

	frag.color = glm::vec4(200 * intensity, 200 * intensity, 200 * intensity, 255);

}
