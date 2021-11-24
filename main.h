#pragma once
#ifndef __MAIN_H__
#define __MAIN_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Math.h"

#include "Entity.h"
#include "pipeline.h"

#endif // !__MAIN_H__
