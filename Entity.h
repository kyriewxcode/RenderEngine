#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "mathtool.h"
#include "model.h"

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
	Transform transform;
	Model* model;
	Entity(Transform trans, Model* mod) :transform(trans), model(mod) {};
};

#endif // !__ENTITY_H__
