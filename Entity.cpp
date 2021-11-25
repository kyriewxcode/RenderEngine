#include "Screen.h"
#include "Entity.h"

Entity::Entity(const char* filename)
{
	loadObj(filename);
}

glm::mat4 matRotate(1);

glm::mat4 Entity::modelMatrix()
{
	glm::mat4 Model(1);
	Model = glm::translate(Model, transform.position);
	Model = glm::scale(Model, transform.scale);
	Model = glm::rotate(Model, glm::radians(45.f), transform.right());
	Model = glm::rotate(Model, glm::radians(45.f), transform.up());;
	Model = glm::rotate(Model, glm::radians(0.f), transform.forward());
	Model = Model * matRotate;
	return Model;
}

void Entity::update()
{
	if (Screen::m_keys[VK_LEFT])
	{
		matRotate = glm::rotate(matRotate, glm::radians(5.f), transform.up());;
	}
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
	// loop over face. 3 = triangle, 4 = quad
	m_indices = shapes[0].points.indices;
	int index_offset = 0;
	for (int f = 0; f < shapes[0].mesh.num_face_vertices.size(); f++)
	{
		Triangle* t = new Triangle();
		int fv = int(shapes[0].mesh.num_face_vertices[f]);
		//loop over vertex
		for (int v = 0; v < fv; v++)
		{
			auto idx = shapes[0].mesh.indices[index_offset + v];

			float vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
			float vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
			float vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
			t->setVertex(v, Vector(vx, vy, vz, 1));

			if (idx.normal_index >= 0)
			{
				float nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
				float ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
				float nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
				t->setNormal(v, Normal(nx, ny, nz, 1));
			}

			if (idx.texcoord_index >= 0)
			{
				float tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
				float ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
				t->setTexcoord(v, Texcoord(tx, ty));
			}

			float r = attrib.colors[3 * size_t(idx.vertex_index) + 0];
			float g = attrib.colors[3 * size_t(idx.vertex_index) + 1];
			float b = attrib.colors[3 * size_t(idx.vertex_index) + 2];
			t->setColor(v, Color(r, g, b, 1));
		}
		m_triangles.push_back(t);
		index_offset += fv;
	}
}

