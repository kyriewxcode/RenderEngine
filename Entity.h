#pragma once
#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "Shader.h"
#include "Transform.h"

class Entity
{
public:
	Transform transform;
	Shader shader;

	std::vector<tinyobj::index_t> m_indices;
	std::vector<Vertex> m_vertexs;
	//std::vector<glm::vec4> m_verts;
	//std::vector<glm::vec3> m_normals;
	//std::vector<glm::vec2> m_texcoords;

public:
	Entity(const char* filename);
	glm::mat4 modelMatrix();
	void update();

private:
	void loadObj(const char* filename);
};

#endif // !__ENTITY_H__
