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

// ��ɫ�����ݽṹ�壬�� ������ɫ�� ���� ƬԪ��ɫ��
struct v2f
{
	std::map<int, float> varying_float;		// �������б�
	std::map<int, Vector2f> varying_vec2f;	// ��ά�����б�
	std::map<int, Vector3f> varying_vec3f;	// ��ά�����б�
	std::map<int, Vector4f> varying_vec4f;	// ��ά�����б�
};

typedef std::function<Vector4f(int index, v2f& output)> VertexShader;

typedef std::function<Vector4f(v2f& input)> FragmentShader;

#pragma endregion


class RenderEngine
{

};

