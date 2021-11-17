#ifndef __ENTITY_H__
#define __ENTITY_H__
#include "mathtool.h"
#include "model.h"

struct Transform
{
	Vector3f position;
	Vector3f rotation;
	Vector3f scale;
	Transform() {};
	Transform(Vector3f pos, Vector3f rotat, Vector3f scal) :position(pos), rotation(rotat), scale(scal) {};
};

struct Entity
{
	Transform transform;
	Model* model;
	Entity(Transform trans, Model* mod) :transform(trans), model(mod) {};
};

#endif // !__ENTITY_H__
