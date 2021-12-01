#pragma once
#include "Math.h"
#include "Device.h"
#include "Shader.h"
#include "Triangle.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include <vector>

class Pipeline
{
public:
	Pipeline(Device* device);
	void addEntity(Entity* entity);
	void switchMode();
	void draw();

private:
	bool shouldCullBack(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3);
	void transformClip2NDC(glm::vec4& vert);
	void transformNDC2screen(glm::vec4& vert);
	bool insideTriangle(int x, int y, const glm::vec4* _v);
	std::tuple<float, float, float> computeBarycentric2D(float x, float y, const glm::vec4* v);
	void drawTriangle(glm::vec4 positionWS[3], glm::vec4 screenPos[3], glm::vec3 normal[3], glm::vec2 texcoord[3]);

private:
	Camera camera;
	Light light;
	std::vector<Entity*> entities;
	std::vector<Triangle*> triangls;
	Device* m_device;
	bool m_texMode = true;
};
