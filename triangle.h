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
		normal[index] = n;
	}

	void setTexcoord(int index, Texcoord uv)
	{
		texcoord[index] = uv;
	}

	void setColor(int index, Color c)
	{
		color[index] = c;
	}

	Normal getNormal()
	{
		return (normal[0] + normal[1] + normal[2]) / 3.f;
	}
};
