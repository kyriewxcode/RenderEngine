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
	void addEntity(Entity entity);
	void switchMode();
	void draw();

private:
	bool cullBackface(const Vector& v1, const Vector& v2, const Vector& v3);
	void transformClip2NDC(Vector& vert);
	void transformNDC2screen(Vector& vert);
	bool insideTriangle(int x, int y, const Vector* _v);
	std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector* v);
	void drawTriangle(Vector& v1, Vector& v2, Vector& v3, Normal normal);

private:
	Camera camera;
	Light light;
	std::vector<Entity> entities;
	Device* m_device;
	bool m_texMode = true;
};
