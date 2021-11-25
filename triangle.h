#pragma once
#include"Math.h"

class Triangle
{
public:
	Vector vertexs[3];
	Normal normal[3];
	Texcoord texcoord[3];
	Color color[3];

public:
	Triangle() {};
	Triangle(Vector p_v0, Vector p_v1, Vector p_v2)
	{
		vertexs[0] = p_v0;
		vertexs[1] = p_v1;
		vertexs[2] = p_v2;
	}

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

	void setColor(int index, Color c)
	{
		color[index] = c;
	}

	Normal worldNormal(glm::mat4 mvp)
	{
		auto n0 = normal[0] * mvp;
		auto n1 = normal[1] * mvp;
		auto n2 = normal[2] * mvp;

		return (n0 + n1 + n2) / 3.f;
	}

	Normal getNormal()
	{
		return (normal[0] + normal[1] + normal[2]) / 3.f;
	}
};
