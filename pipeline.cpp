#include "Pipeline.h"
#include <algorithm>
#include <array>

#undef max
#undef min

Pipeline::Pipeline(Device* device)
{
	m_device = device;
}

void Pipeline::addEntity(Entity* entity)
{
	entities.push_back(entity);
}

void Pipeline::switchMode()
{
	m_texMode = !m_texMode;
}

bool Pipeline::shouldCullBack(const Vector& v1, const Vector& v2, const Vector& v3)
{
	glm::vec3 tmp1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
	glm::vec3 tmp2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
	glm::vec3 normal = glm::cross(tmp1, tmp2);
	glm::vec3 view = camera.transform.forward();
	float res = glm::dot(view, normal);
	if (res < 0)
		return true;
	else
		return false;
}

void Pipeline::transformClip2NDC(Vector& vert)
{
	vert.x /= vert.w;
	vert.y /= vert.w;
	vert.z /= vert.w;
}

void Pipeline::transformNDC2screen(Vector& vert)
{
	float map = (vert.x + 1.f) / 2.f;
	vert.x = WIDTH * map;
	map = 1.f - ((vert.y + 1.f) / 2.f);
	vert.y = HEIGHT * map;

	float f1 = (50.f - 0.1f) / 2.0f;
	float f2 = (50.f + 0.1f) / 2.0f;
	vert.z = vert.z * f1 + f2;
}

bool Pipeline::insideTriangle(int x, int y, const Vector* _v)
{
	glm::vec3 v[3];
	for (int i = 0; i < 3; i++)
		v[i] = { _v[i].x,_v[i].y, 1.0 };
	glm::vec3 f0, f1, f2;
	f0 = glm::cross(v[1], v[0]);
	f1 = glm::cross(v[2], v[1]);
	f2 = glm::cross(v[0], v[2]);
	glm::vec3 p(x, y, 1.);
	if ((glm::dot(p, f0) * glm::dot(f0, v[2]) >= 0) && (glm::dot(p, f1) * glm::dot(f1, v[0]) >= 0) && (glm::dot(p, f2) * dot(f2, v[1]) >= 0))
		return true;

	return false;
}

std::tuple<float, float, float> Pipeline::computeBarycentric2D(float x, float y, const Vector* v)
{
	float c1 = (x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * y + v[1].x * v[2].y - v[2].x * v[1].y) /
		(v[0].x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * v[0].y + v[1].x * v[2].y - v[2].x * v[1].y);
	float c2 = (x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * y + v[2].x * v[0].y - v[0].x * v[2].y) /
		(v[1].x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * v[1].y + v[2].x * v[0].y - v[0].x * v[2].y);
	float c3 = (x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * y + v[0].x * v[1].y - v[1].x * v[0].y) /
		(v[2].x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * v[2].y + v[0].x * v[1].y - v[1].x * v[0].y);
	return { c1,c2,c3 };
}

void Pipeline::draw()
{
	for (auto& entity : entities)
	{
		auto model = entity->modelMatrix();
		auto view = camera.viewMatrix();
		auto project = camera.projectionMatrix();
		auto mvp = project * view * model;

		// TODO: vertex shader
		for (auto& triangle : entity->m_triangles)
		{
			Vector newVertex[3]
			{
				mvp * triangle->vertexs[0],
				mvp * triangle->vertexs[1],
				mvp * triangle->vertexs[2]
			};

			for (int i = 0; i < 3; i++)
			{
				transformClip2NDC(newVertex[i]);
			}

			if (shouldCullBack(newVertex[0], newVertex[1], newVertex[2])) continue;

			for (int i = 0; i < 3; i++)
			{
				transformNDC2screen(newVertex[i]);
			}

			drawTriangle(newVertex[0], newVertex[1], newVertex[2], glm::normalize(triangle->getNormal()));
		}
	}
}

float sature(float n)
{
	n = std::max(0.f, n);
	n = std::min(1.f, n);
	return n;
}

void Pipeline::drawTriangle(Vector& v1, Vector& v2, Vector& v3, Normal normal)
{
	glm::vec3 lightDir = light.transform.position;
	Vector v[3]{ v1,v2,v3 };
	auto colorrand = glm::vec4(rand() % 100 / (double)101, rand() % 100 / (double)101, rand() % 100 / (double)101, 1);

	int top = (int)ceil(std::max(v1.y, std::max(v2.y, v3.y)));
	int bottom = (int)floor(std::min(v1.y, std::min(v2.y, v3.y)));
	int left = (int)floor(std::min(v1.x, std::min(v2.x, v3.x)));
	int right = (int)ceil(std::max(v1.x, std::max(v2.x, v3.x)));

	for (int x = left; x <= right; x++)
	{
		for (int y = bottom; y <= top; y++)
		{
			if (insideTriangle(x, y, v))
			{
				// P = alpha*A + beta*B + gamma*C
				auto [alpha, beta, gamma] = computeBarycentric2D(x, y, v);
				float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w; // compute point.z with w
				float Z = 1.0f / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w); // compute point.w
				zp *= Z;
				if (zp > m_device->getZbuffer(x, y))
				{
					m_device->setZbuffer(x, y, zp);

					float intensity = sature(glm::dot((glm::vec3)normal, lightDir));
					intensity *= 0.8;
					intensity += 0.2;
					Color c = Color(245 * intensity, 245 * intensity, 220 * intensity, 255);
					m_device->drawPixel(x, y, c);
				}
			}
		}
	}
}