#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "math.h"

struct Transform
{
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);;
	glm::vec3 scale = glm::vec3(0, 0, 0);;
	Transform() {};
	Transform(glm::vec3 pos, glm::vec3 rotat, glm::vec3 scal) :position(pos), rotation(rotat), scale(scal) {};
};

struct Entity
{
	const char* inputfile;
	Transform transform;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector<Triangle*> triangles;

	Entity(const char* filename) :inputfile(filename)
	{
		loadObj();
		set_triangles();
	};

	void loadObj()
	{
		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile, NULL, true);
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
	}

	void set_triangles()
	{
		// loop over shapes
		for (size_t s = 0; s < shapes.size(); s++)
		{
			size_t index_offset = 0;
			// loop over faces(polygon)
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
			{
				Triangle* t = new Triangle();
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
				//loop over vertices in the face
				for (size_t v = 0; v < fv; v++)
				{
					// vertex data
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
					t->setVertex(v, glm::vec4(vx, vy, vz, 1));

					// normal data
					if (idx.normal_index >= 0)
					{
						tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
						tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
						tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
						t->setNormal(v, glm::vec3(nx, ny, nz));
					}

					// texcoord data
					if (idx.texcoord_index >= 0)
					{
						tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
						t->setTexCoord(v, glm::vec2(tx, ty));
					}

					// vertex colors
					tinyobj::real_t red = attrib.colors[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t green = attrib.colors[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t blue = attrib.colors[3 * size_t(idx.vertex_index) + 2];
					t->setColor(v, red, green, blue);
				}
				triangles.push_back(t);
				index_offset += fv;
				shapes[s].mesh.material_ids[f];
			}
		}
	}
};

#endif // !__ENTITY_H__
