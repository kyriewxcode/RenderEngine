#pragma once
#include "math.h"

class Vertex
{
public:
	Vector pos;
	Normal normal;
	Texcoord texcoord;
public:
	Vertex() {}
	Vertex(Vector p, Normal n, Texcoord t) :pos(p), normal(n), texcoord(t) {}
};

