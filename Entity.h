#pragma once
#include "Math.h"
#include "Transform.h"
#include "Shader.h"
#include "Triangle.h"

class Entity
{
public:
	Transform transform;
	std::vector<tinyobj::index_t> m_indices;
	std::vector<Triangle*> m_triangles;
	Shader shader;

public:
	Entity(const char* filename);
	glm::mat4 modelMatrix();

private:
	void loadObj(const char* filename);
};

