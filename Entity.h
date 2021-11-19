#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "mathtool.h"

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
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

	Entity(const char* filename) :inputfile(filename)
	{
		loadObj();
		transform.position = glm::vec3(0, 0, 0);
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
};

#endif // !__ENTITY_H__
