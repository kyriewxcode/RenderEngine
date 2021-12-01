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

bool Pipeline::shouldCullBack(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3)
{
	glm::vec3 tmp1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
	glm::vec3 tmp2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
	glm::vec3 normal = glm::cross(tmp1, tmp2);
	float res = glm::dot(-vecForward, normal);
	if (res > 0)
		return true;
	else
		return false;
}

void Pipeline::transformClip2NDC(glm::vec4& vert)
{
	vert.x /= vert.w;
	vert.y /= vert.w;
	vert.z /= vert.w;
}

void Pipeline::transformNDC2screen(glm::vec4& vert)
{
	float map = (vert.x + 1.f) / 2.f;
	vert.x = WIDTH * map;
	map = 1.f - ((vert.y + 1.f) / 2.f);
	vert.y = HEIGHT * map;

	float f1 = (50.f - 0.1f) / 2.0f;
	float f2 = (50.f + 0.1f) / 2.0f;
	vert.z = vert.z * f1 + f2;
}

bool Pipeline::insideTriangle(int x, int y, const glm::vec4* _v)
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

std::tuple<float, float, float> Pipeline::computeBarycentric2D(float x, float y, const glm::vec4* v)
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
		entity->shader.setModelMat(model);
		entity->shader.setMatMVP(mvp);


		// set up triangle buffer
		triangls.clear();
		for (int i = 0; i < entity->m_indices.size(); i += 3)
		{
			Triangle* t = new Triangle();
			for (int j = 0; j < 3; j++)
			{
				auto index = i + j;

				//entity->shader.vertexShader(entity->m_verts[index], entity->m_normals[index]);
				t->setVertex(j, entity->m_verts[index]);
				if (entity->m_normals.size() > 0)
					t->setNormal(j, entity->m_normals[index]);
				if (entity->m_texcoords.size() > 0)
					t->setTexcoord(j, entity->m_texcoords[index]);
			}
			triangls.push_back(t);
		}

		glm::vec3 viewDir = camera.transform.position - entity->transform.position;

		for (auto& triangle : triangls)
		{
			glm::vec4 positionWS[3]
			{
				mvp * triangle->vertexs[0],
				mvp * triangle->vertexs[1],
				mvp * triangle->vertexs[2]
			};

			glm::vec3 worldNormal[3]
			{
				triangle->normal[0] * (glm::mat3x3)glm::inverse(model),
				triangle->normal[1] * (glm::mat3x3)glm::inverse(model),
				triangle->normal[2] * (glm::mat3x3)glm::inverse(model),
			};

			glm::vec2 newTexcoord[3]
			{
				triangle->texcoord[0],
				triangle->texcoord[1],
				triangle->texcoord[2]
			};

			glm::vec4 screenPos[3]
			{
				positionWS[0],
				positionWS[1],
				positionWS[2]
			};

			for (int i = 0; i < 3; i++)
			{
				transformClip2NDC(screenPos[i]);
			}

			if (shouldCullBack(screenPos[0], screenPos[1], screenPos[2])) continue;

			for (int i = 0; i < 3; i++)
			{
				transformNDC2screen(screenPos[i]);
			}

			drawTriangle(positionWS, screenPos, worldNormal, newTexcoord);
		}
	}
}

float sature(float n)
{
	n = std::max(0.f, n);
	n = std::min(1.f, n);
	return n;
}

void Pipeline::drawTriangle(glm::vec4 positionWS[3], glm::vec4 screenPos[3], glm::vec3 normals[3], glm::vec2 texcoord[3])
{
	auto colorrand = glm::vec4(rand() % 100 / (double)101, rand() % 100 / (double)101, rand() % 100 / (double)101, 1);

	int top = (int)ceil(std::max(screenPos[0].y, std::max(screenPos[1].y, screenPos[2].y)));
	int bottom = (int)floor(std::min(screenPos[0].y, std::min(screenPos[1].y, screenPos[2].y)));
	int left = (int)floor(std::min(screenPos[0].x, std::min(screenPos[1].x, screenPos[2].x)));
	int right = (int)ceil(std::max(screenPos[0].x, std::max(screenPos[1].x, screenPos[2].x)));

	for (int x = left; x <= right; x++)
	{
		for (int y = bottom; y <= top; y++)
		{
			if (insideTriangle(x, y, screenPos))
			{
				// P = alpha * A + beta * B + gamma * C
				auto [alpha, beta, gamma] = computeBarycentric2D(x, y, screenPos);

				glm::vec3 worldPos = glm::vec3(alpha * positionWS[0] + beta * positionWS[1] + gamma * positionWS[2]);
				float depth = alpha * screenPos[0].z + beta * screenPos[1].z + gamma * screenPos[2].z;
				glm::vec3 normal = glm::normalize(alpha * normals[0] + beta * normals[1] + gamma * normals[2]);

				glm::vec3 lightDir = glm::normalize(light.transform.position - worldPos);
				if (depth > m_device->getZbuffer(x, y))
				{
					m_device->setZbuffer(x, y, depth);
					float intensity = sature(glm::dot(lightDir, normal));
					intensity += 0.2;
					glm::vec4 c = glm::vec4(200 * intensity, 200 * intensity, 200 * intensity, 255);
					m_device->drawPixel(x, y, c);
				}
			}
		}
	}
}