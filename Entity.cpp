#include "Screen.h"
#include "Entity.h"

Entity::Entity(const char* filename)
{
	loadObj(filename);
}

glm::mat4 Entity::modelMatrix()
{
	glm::mat4 Model(1);
	Model = glm::translate(Model, transform.position);
	Model = glm::scale(Model, transform.scale);
	Model = glm::rotate(Model, glm::radians(transform.eulerAngles.x), vecRight);
	Model = glm::rotate(Model, glm::radians(transform.eulerAngles.y), vecUp);;
	Model = glm::rotate(Model, glm::radians(transform.eulerAngles.z), vecForward);
	return Model;
}

void Entity::update()
{
	if (Screen::m_keys[VK_LEFT])
	{
		transform.eulerAngles.y -= 5.f;
	}
	if (Screen::m_keys[VK_RIGHT])
	{
		transform.eulerAngles.y += 5.f;
	}
	if (Screen::m_keys[VK_UP])
	{
		transform.eulerAngles.x += 5.f;
	}
	if (Screen::m_keys[VK_DOWN])
	{
		transform.eulerAngles.x -= 5.f;
	}

	if (transform.eulerAngles.x > 360.f || transform.eulerAngles.x < -360.f) transform.eulerAngles.x = 0;
	if (transform.eulerAngles.y > 360.f || transform.eulerAngles.y < -360.f) transform.eulerAngles.y = 0;
}

void Entity::loadObj(const char* filename)
{
	std::string warn;
	std::string err;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, NULL, true);
	if (!warn.empty())
	{
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty())
	{
		std::cerr << "ERR: " << err << std::endl;
	}

	if (!ret)
	{
		printf("Failed to load/parse .obj.\n");
		exit(1);
	}

	// only load one shape
	int index_offset = 0;
	m_indices = shapes[0].mesh.indices;
	for (int f = 0; f < shapes[0].mesh.num_face_vertices.size(); f++)
	{
		int fv = int(shapes[0].mesh.num_face_vertices[f]);
		//loop over vertex
		for (int v = 0; v < fv; v++)
		{
			auto idx = shapes[0].mesh.indices[index_offset + v];
			Vertex vert;

			float vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
			float vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
			float vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
			//m_verts.push_back(glm::vec4(vx, vy, vz, 1));
			vert.pos = glm::vec4(vx, vy, vz, 1);


			if (idx.normal_index >= 0)
			{
				float nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
				float ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
				float nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
				//m_normals.push_back(glm::vec3(nx, ny, nz));
				vert.normal = glm::vec3(nx, ny, nz);
			}
			else
			{
				float nx = attrib.normals[3 * size_t(idx.vertex_index) + 0];
				float ny = attrib.normals[3 * size_t(idx.vertex_index) + 1];
				float nz = attrib.normals[3 * size_t(idx.vertex_index) + 2];
				//m_normals.push_back(glm::vec3(nx, ny, nz));
				vert.normal = glm::vec3(nx, ny, nz);
			}

			if (idx.texcoord_index >= 0)
			{
				float tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
				float ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
				//m_texcoords.push_back(glm::vec2(tx, ty));
				vert.texcoord = glm::vec2(tx, ty);
			}

			m_vertexs.push_back(vert);
		}
		index_offset += fv;
	}
}

