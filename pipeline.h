#pragma once
#include "math.h"
#include "Device.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"

class Pipeline
{
public:
	Pipeline(Device* device);

	void addEntity(Entity& entity);

	void draw();

private:
	bool shouldCullBack(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3);

	bool insideTriangle(int x, int y, const glm::vec3 screenPos[3]);

	std::tuple<float, float, float> computeBarycentric2D(float x, float y, const glm::vec3* v);

	void rasterization(Entity& entity);

private:
	Camera camera;
	Light light;

	std::vector<Entity> entities;
	std::vector<Triangle> triangls;
	std::vector<ShaderContex> vertexContex;
	std::vector<ShaderContex> fragmentContex;

	Device* m_device;
};
