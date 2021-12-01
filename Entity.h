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
	std::vector<glm::vec4> m_verts;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_texcoords;

public:
	Entity(const char* filename);
	glm::mat4 modelMatrix();
	void update();

private:
	void loadObj(const char* filename);
};

