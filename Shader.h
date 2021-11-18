#include <vector>
#include "mathtool.h"
#include "Entity.h"
#ifndef __SHADER_H__
#define __SHADER_H__


class Shader
{
public:
	glm::vec4 vertexShader(const int iface,const int nvert);
	TGAColor fragmentShader(glm::vec4 vert);
};

#endif // !__SHADER_H__

