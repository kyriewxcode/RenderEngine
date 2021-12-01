#pragma once
#include"Math.h"

class Triangle
{
public:
	std::vector<glm::vec4> vertexs;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec2> texcoord;

public:
	Triangle()
	{
		vertexs = std::vector<glm::vec4>(3);
		normal = std::vector<glm::vec3>(3);
		texcoord = std::vector<glm::vec2>(3);
	};

	void setVertex(int index, glm::vec4 v)
	{
		vertexs[index] = v;
	}

	void setNormal(int index, glm::vec3 n)
	{
		normal[index] = glm::normalize(n);
	}

	void setTexcoord(int index, glm::vec2 uv)
	{
		texcoord[index] = uv;
	}
};
