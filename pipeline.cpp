#pragma once
#include "Pipeline.h"

#undef max
#undef min

Pipeline::Pipeline(Device* device)
{
	m_device = device;
}

void Pipeline::addEntity(Entity& entity)
{
	entities.push_back(entity);
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

bool Pipeline::insideTriangle(int x, int y, const glm::vec3 screenPos[3])
{
	glm::vec3 v[3];
	for (int i = 0; i < 3; i++)
		v[i] = { screenPos[i].x,screenPos[i].y, 1.0 };
	glm::vec3 f0, f1, f2;
	f0 = glm::cross(v[1], v[0]);
	f1 = glm::cross(v[2], v[1]);
	f2 = glm::cross(v[0], v[2]);
	glm::vec3 p(x, y, 1.);
	if ((glm::dot(p, f0) * glm::dot(f0, v[2]) >= 0) && (glm::dot(p, f1) * glm::dot(f1, v[0]) >= 0) && (glm::dot(p, f2) * dot(f2, v[1]) >= 0))
		return true;

	return false;
}

std::tuple<float, float, float> Pipeline::computeBarycentric2D(float x, float y, const glm::vec3* v)
{
	float c1 = (x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * y + v[1].x * v[2].y - v[2].x * v[1].y) /
		(v[0].x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * v[0].y + v[1].x * v[2].y - v[2].x * v[1].y);
	float c2 = (x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * y + v[2].x * v[0].y - v[0].x * v[2].y) /
		(v[1].x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * v[1].y + v[2].x * v[0].y - v[0].x * v[2].y);
	float c3 = (x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * y + v[0].x * v[1].y - v[1].x * v[0].y) /
		(v[2].x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * v[2].y + v[0].x * v[1].y - v[1].x * v[0].y);
	return { c1,c2,c3 };
}

float sature(float n)
{
	n = std::max(0.f, n);
	n = std::min(1.f, n);
	return n;
}

void Pipeline::draw()
{
	auto view = camera.viewMatrix();
	auto project = camera.projectionMatrix();

	for (auto& entity : entities)
	{
		entity.update();
		fragmentContex.clear();
		vertexContex.clear();
		triangls.clear();

		// get mvp matrix
		auto model = entity.modelMatrix();
		auto mvp = project * view * model;
		entity.shader.setModelMat(model);
		entity.shader.setMatMVP(mvp);

		// vertex shader
		for (auto& v : entity.m_vertexs)
		{
			vertexContex.push_back(entity.shader.vertexShader(v));
		}

		// rasterization
		rasterization(entity);
	}
}

void Pipeline::rasterization(Entity& entity)
{
	// set up triangle
	for (int i = 0; i < entity.m_indices.size(); i += 3)
	{
		Triangle t;
		for (int j = 0; j < 3; j++)
		{
			auto index = i + j;
			t.vertexContexs[j] = vertexContex[index];
		}
		if (shouldCullBack(t.vertexContexs[0].clipPos, t.vertexContexs[1].clipPos, t.vertexContexs[2].clipPos)) continue;
		triangls.push_back(t);
	}

	// traversal triangle
	for (auto& t : triangls)
	{
		auto& vertexA = t.vertexContexs[0];
		auto& vertexB = t.vertexContexs[1];
		auto& vertexC = t.vertexContexs[2];

		int top = (int)ceil(std::max(vertexA.screenPos.y, std::max(vertexB.screenPos.y, vertexC.screenPos.y)));
		int bottom = (int)floor(std::min(vertexA.screenPos.y, std::min(vertexB.screenPos.y, vertexC.screenPos.y)));
		int left = (int)floor(std::min(vertexA.screenPos.x, std::min(vertexB.screenPos.x, vertexC.screenPos.x)));
		int right = (int)ceil(std::max(vertexA.screenPos.x, std::max(vertexB.screenPos.x, vertexC.screenPos.x)));

		glm::vec3 triangleVertexs[3]
		{
			vertexA.screenPos,
			vertexB.screenPos,
			vertexC.screenPos
		};

		for (int y = bottom; y <= top; y++)
		{
			for (int x = left; x <= right; x++)
			{
				if (insideTriangle(x, y, triangleVertexs))
				{
					ShaderContex fragContex;
					fragContex.x = x;
					fragContex.y = y;

					// P = alpha * A + beta * B + gamma * C
					auto [alpha, beta, gamma] = computeBarycentric2D(x, y, triangleVertexs);
					fragContex.worldPos = alpha * vertexA.worldPos + beta * vertexB.worldPos + gamma * vertexC.worldPos;
					fragContex.normal = glm::normalize(alpha * vertexA.normal + beta * vertexB.normal + gamma * vertexC.normal);
					fragContex.depth = alpha * vertexA.depth + beta * vertexB.depth + gamma * vertexC.depth;
					fragContex.texcoord = alpha * vertexA.texcoord + beta * vertexB.texcoord + gamma * vertexC.texcoord;

					fragmentContex.push_back(fragContex);
				}
			}
		}
	}

	// fragment shader
	for (auto& frag : fragmentContex)
	{
		entity.shader.fragmentShader(frag);
	}

	// depth test
	std::vector<ShaderContex> pixel;
	for (auto& frag : fragmentContex)
	{
		if (frag.depth < m_device->getZbuffer(frag.x, frag.y))
		{
			m_device->setZbuffer(frag.x, frag.y, frag.depth);
			pixel.push_back(frag);
		}
	}

	// draw
	for (auto& p : pixel)
	{
		m_device->drawPixel(p.x, p.y, p.color);
	}
}