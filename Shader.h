#include <vector>
#include "mathtool.h"
#include "Entity.h"
#ifndef __SHADER_H__
#define __SHADER_H__


class Shader
{
public:
	Vector4f vertexShader(const int iface,const int nvert);
	TGAColor fragmentShader(Vector4f vert);
};

#endif // !__SHADER_H__

