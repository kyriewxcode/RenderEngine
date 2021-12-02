#pragma once

#include "tiny_obj_loader.h"
#include "math.h"

struct ShaderContex
{
	int x, y;
	glm::vec4 color;

	float depth;
	glm::vec4 clipPos;
	glm::vec3 screenPos;
	glm::vec3 worldPos;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

struct Vertex
{
	glm::vec4 pos;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

struct Triangle
{
	std::vector<ShaderContex> vertexContexs;
	Triangle()
	{
		vertexContexs = std::vector<ShaderContex>(3);
	}
};
