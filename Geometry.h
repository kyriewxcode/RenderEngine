#pragma once
#ifndef __GEOMETRY_H_
#define __GEOMETRY_H_

#include "tiny_obj_loader.h"
#include "math.h"

struct ShaderContex
{
	float depth;
	glm::vec4 clipPos;
	glm::vec3 screenPos;
	glm::vec3 worldPos;
	glm::vec3 normal;
	glm::vec2 texcoord;
	glm::vec4 color;
	int x, y;
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
	int index[3];
	Triangle()
	{
		vertexContexs = std::vector<ShaderContex>(3);
	}
};
#endif // !__GEOMETRY_H_
