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
	auto view = camera.viewMatrix();
	auto project = camera.projectionMatrix();
	for (auto& entity : entities)
	{
		auto model = entity->modelMatrix();
		auto mvp = project * view * model;

		// set up triangle buffer
		triangls.clear();
		for (int i = 0; i < entity->m_indices.size(); i += 3)
		{
			Triangle* t = new Triangle();
			for (int j = 0; j < 3; j++)
			{
				t->setVertex(j, entity->m_verts[i + j]);
				t->setNormal(j, entity->m_normals[i + j]);
				t->setTexcoord(j, entity->m_texcoords[i + j]);
			}
			triangls.push_back(t);
		}

		for (auto& triangle : triangls)
		{
			Vector newVertex[3]
			{
				mvp * triangle->vertexs[0],
				mvp * triangle->vertexs[1],
				mvp * triangle->vertexs[2]
			};

			Normal worldNormal[3]
			{
				triangle->normal[0] * (glm::mat3x3)glm::inverse(model),
				triangle->normal[1] * (glm::mat3x3)glm::inverse(model),
				triangle->normal[2] * (glm::mat3x3)glm::inverse(model),
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

			drawTriangle(newVertex, worldNormal);
		}
	}
}

float sature(float n)
{
	n = std::max(0.f, n);
	n = std::min(1.f, n);
	return n;
}

void Pipeline::drawTriangle(Vector vertexs[3], Normal normals[3])
{
	glm::vec3 lightDir = light.transform.position;
	auto colorrand = glm::vec4(rand() % 100 / (double)101, rand() % 100 / (double)101, rand() % 100 / (double)101, 1);

	int top = (int)ceil(std::max(vertexs[0].y, std::max(vertexs[1].y, vertexs[2].y)));
	int bottom = (int)floor(std::min(vertexs[0].y, std::min(vertexs[1].y, vertexs[2].y)));
	int left = (int)floor(std::min(vertexs[0].x, std::min(vertexs[1].x, vertexs[2].x)));
	int right = (int)ceil(std::max(vertexs[0].x, std::max(vertexs[1].x, vertexs[2].x)));

	for (int x = left; x <= right; x++)
	{
		for (int y = bottom; y <= top; y++)
		{
			if (insideTriangle(x, y, vertexs))
			{
				// P = alpha * A + beta * B + gamma * C
				auto [alpha, beta, gamma] = computeBarycentric2D(x, y, vertexs);
				Normal normal = glm::normalize(alpha * normals[0] + beta * normals[1] + gamma * normals[2]);
				float depth = alpha * vertexs[0].z + beta * vertexs[1].z + gamma * vertexs[2].z;
				if (depth > m_device->getZbuffer(x, y))
				{
					m_device->setZbuffer(x, y, depth);
					float intensity = sature(glm::dot((glm::vec3)normal, lightDir));
					intensity += 0.2;
					Color c = Color(200 * intensity, 200 * intensity, 200 * intensity, 255);
					m_device->drawPixel(x, y, c);
				}
			}
		}
	}
}