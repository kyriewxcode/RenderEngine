#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <map>
#include <initializer_list>
#include <stdexcept>
#include <functional>
#include <ostream>
#include <sstream>
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;

#pragma region Shader

// 着色器数据结构体，由 顶点着色器 传到 片元着色器
struct v2f
{
	std::map<int, float> varying_float;		// 浮点数列表
	std::map<int, Vector2f> varying_vec2f;	// 二维向量列表
	std::map<int, Vector3f> varying_vec3f;	// 三维向量列表
	std::map<int, Vector4f> varying_vec4f;	// 四维向量列表
};

typedef std::function<Vector4f(int index, v2f& output)> VertexShader;

typedef std::function<Vector4f(v2f& input)> FragmentShader;

#pragma endregion


class RenderEngine
{

};

