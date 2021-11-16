#ifndef __PIPELINE_H__
#define __PIPELINE_H__
#include "tgaimage.h"
#include "mathtool.h"
#include "Shader.h"


Matrix4f ModelView;
Matrix4f Viewport;
Matrix4f Projection;
Vector3f cameraPos;

class Pipeline
{
public:
	Pipeline(Shader vertex, Shader fragment) : vert(vertex), frag(fragment) {};

private:
	Shader vert;
	Shader frag;
};

#endif // !__PIPELINE_H__
