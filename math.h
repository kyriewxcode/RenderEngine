#pragma once
#ifndef __MATH_TOOL_H__
#define __MATH_TOOL_H__

//#include <Eigen/Dense>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <vector>

#include "Timer.h"
#include "tiny_obj_loader.h"

typedef unsigned int UINT32;

typedef glm::vec2 Texcoord;
typedef glm::vec4 Normal;
typedef glm::vec4 Color;
typedef glm::vec4 Vector;

const glm::vec3 vecZero = glm::vec3(0, 0, 0);
const glm::vec3 vecOne = glm::vec3(1, 1, 1);
const glm::vec3 vecRight = glm::vec3(1, 0, 0);
const glm::vec3 vecUp = glm::vec3(0, 1, 0);
const glm::vec3 vecForward = glm::vec3(0, 0, 1);

const int WIDTH = 800;
const int HEIGHT = 600;

#endif // !__MATH_TOOL_H__

