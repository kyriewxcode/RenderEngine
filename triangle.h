#pragma once
#include"Math.h"

class Triangle
{
public:
	std::vector<Vector> vertexs;
	std::vector<Normal> normal;
	std::vector<Texcoord> texcoord;

public:
	Triangle()
	{
		vertexs = std::vector<Vector>(3);
		normal = std::vector<Normal>(3);
		texcoord = std::vector<Texcoord>(3);
	};

	void setVertex(int index, Vector v)
	{
		vertexs[index] = v;
	}

	void setNormal(int index, Normal n)
	{
		normal[index] = glm::normalize(n);
	}

	void setTexcoord(int index, Texcoord uv)
	{
		texcoord[index] = uv;
	}

	Normal getNormal()
	{
		return (normal[0] + normal[1] + normal[2]) / 3.f;
	}
};
