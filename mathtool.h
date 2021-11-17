#ifndef __MATH_TOOL_H__
#define __MATH_TOOL_H__

#include "Eigen/Dense"
#include <iostream>

typedef Eigen::Matrix4f Matrix4f;
typedef Eigen::Vector2f Vector2f;
typedef Eigen::Vector3f Vector3f;
typedef Eigen::Vector4f Vector4f;

#define MY_PI 3.1415926
#define TWO_PI (2.0* MY_PI)
constexpr double DEG_TO_RAD = MY_PI / 180.0;

#endif // !__MATH_TOOL_H__

