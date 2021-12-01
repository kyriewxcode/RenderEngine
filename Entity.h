#pragma once
#include "Math.h"
#include "Transform.h"
#include "Shader.h"
#include "triangle.h"

class Entity
{
public:
	Transform transform;
	Shader shader;

	std::vector<tinyobj::index_t> m_indices;
	std::vector<Vector> m_verts;
	std::vector<Normal> m_normals;
	std::vector<Texcoord> m_texcoords;

public:
	Entity(const char* filename);
	glm::mat4 modelMatrix();
	void update();

private:
	void loadObj(const char* filename);
};

