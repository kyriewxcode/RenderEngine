#include "pipeline.h"
#include <GLFW\glfw3.h>
#include <algorithm>

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const glm::vec4* v)
{
	float c1 = (x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * y + v[1].x * v[2].y - v[2].x * v[1].y) / (v[0].x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * v[0].y + v[1].x * v[2].y - v[2].x * v[1].y);
	float c2 = (x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * y + v[2].x * v[0].y - v[0].x * v[2].y) / (v[1].x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * v[1].y + v[2].x * v[0].y - v[0].x * v[2].y);
	float c3 = (x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * y + v[0].x * v[1].y - v[1].x * v[0].y) / (v[2].x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * v[2].y + v[0].x * v[1].y - v[1].x * v[0].y);
	return { c1,c2,c3 };
}

static bool insideTriangle(int x, int y, const glm::vec4* _v)
{
	glm::vec3 v[3];
	for (int i = 0; i < 3; i++)
		v[i] = { _v[i].x,_v[i].y, 1.0 };
	glm::vec3 f0, f1, f2;
	f0 = glm::cross(v[1], v[0]);
	f1 = glm::cross(v[2], v[1]);
	f2 = glm::cross(v[0], v[2]);
	glm::vec3 p(x, y, 1.);
	if ((glm::dot(p, f0) * glm::dot(f0, v[2]) > 0) && (glm::dot(p, f1) * glm::dot(f1, v[0]) > 0) && (glm::dot(p, f2) * dot(f2, v[1]) > 0))
		return true;

	return false;
}

static glm::vec2 interpolate(float alpha, float beta, float gamma, const glm::vec2& vert1, const glm::vec2& vert2, const glm::vec2& vert3, float weight)
{
	auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
	auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

	u /= weight;
	v /= weight;

	return glm::vec2(u, v);
}

static glm::vec3 interpolate(float alpha, float beta, float gamma, const glm::vec3& vert1, const glm::vec3& vert2, const glm::vec3& vert3, float weight)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

static float sature(float n)
{
	n = std::max(0.f, n);
	n = std::min(1.f, n);
	return n;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

static auto to_vec4(const glm::vec3& v3, float w)
{
	return glm::vec4(v3.x, v3.y, v3.z, w);
}

int Pipeline::get_index(int x, int y)
{
	return (HEIGHT - y) * WIDTH + x;
}

glm::mat4 Pipeline::get_model_matrix(Transform transform)
{
	glm::mat4 Model(1);
	Model = glm::translate(Model, transform.position);
	Model = glm::scale(Model, transform.scale);
	Model = glm::rotate(Model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
	Model = glm::rotate(Model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));;
	Model = glm::rotate(Model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
	return Model;
}

void Pipeline::clear_color(glm::vec4 color)
{
	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			auto index = y * WIDTH + x;
			pixels[index * 4 + 0] = color.r;
			pixels[index * 4 + 1] = color.g;
			pixels[index * 4 + 2] = color.b;
			pixels[index * 4 + 3] = color.a;
		}
	}
}

bool Pipeline::ClipSpaceCull(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3)
{
	if (v1.w < camera.zNear && v2.w < camera.zNear && v3.w < camera.zNear)
		return false;
	if (v1.w > camera.zFar && v2.w > camera.zFar && v3.w > camera.zFar)
		return false;
	if (v1.x > v1.w && v2.x > v2.w && v3.x > v3.w)
		return false;
	if (v1.x < -v1.w && v2.x < -v2.w && v3.x < -v3.w)
		return false;
	if (v1.y > v1.w && v2.y > v2.w && v3.y > v3.w)
		return false;
	if (v1.y < -v1.w && v2.y < -v2.w && v3.y < -v3.w)
		return false;
	if (v1.z > v1.w && v2.z > v2.w && v3.z > v3.w)
		return false;
	if (v1.z < -v1.w && v2.z < -v2.w && v3.z < -v3.w)
		return false;
	return true;
}

bool Pipeline::FaceCull(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3, Face face)
{
	glm::vec3 tmp1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
	glm::vec3 tmp2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);

	glm::vec3 normal = glm::normalize(glm::cross(tmp1, tmp2));
	glm::vec3 view = camera.transform.position; // camera lookat glm::vec3(0,0,0)

	if (face == Face::Back)
		return glm::dot(normal, view) > 0;
	else
		return glm::dot(normal, view) < 0;
}

void Pipeline::render()
{
	// rasterization
	for (const auto entity : entities)
	{
		ModelView = get_model_matrix(entity.transform);
		glm::mat4 mvp = Projection * Viewport * ModelView;
		for (const auto& t : entity.triangles)
		{
			Triangle newtri;

			std::array<glm::vec3, 3> viewspace_pos
			{
				(Viewport * ModelView * t->vert[0]),
				(Viewport * ModelView * t->vert[1]),
				(Viewport * ModelView * t->vert[2])
			};

			glm::vec4 vertex[] = {
					mvp * t->vert[0],
					mvp * t->vert[1],
					mvp * t->vert[2]
			};

			if (!ClipSpaceCull(vertex[0], vertex[1], vertex[2]))
			{
				continue;
			}

			// translate to NDC
			for (auto& v : vertex)
			{
				v.x /= v.w;
				v.y /= v.w;
				v.z /= v.w;
			}

			if (!FaceCull(vertex[0], vertex[1], vertex[2], Face::Back))
			{
				continue;
			}

			glm::mat4 inv_trans = glm::transpose(glm::inverse(Viewport * ModelView));
			glm::vec4 n[] = {
					inv_trans * to_vec4(t->normal[0], 0.0f),
					inv_trans * to_vec4(t->normal[1], 0.0f),
					inv_trans * to_vec4(t->normal[2], 0.0f)
			};

			// viewport transformation
			float f1 = (50.f - 0.1f) / 2.0f;
			float f2 = (50.f + 0.1f) / 2.0f;
			for (auto& v : vertex)
			{
				v.x = 0.5f * (float)WIDTH * (v.x + 1.0f);
				v.y = 0.5f * (float)HEIGHT * (v.y + 1.0f);
				v.z = v.z * f1 + f2;
			}

			for (int i = 0; i < 3; ++i)
			{
				// screen space position
				newtri.setVertex(i, vertex[i]);
				newtri.setNormal(i, glm::vec3(n[i]));
				//newtri.setColor(i, t->color->r, t->color->g, t->color->b);
			}

			triangle(newtri, viewspace_pos);
		}
	}
}

void Pipeline::triangle(const Triangle& t, const std::array<glm::vec3, 3>& view_pos)
{
	auto v = t.toVector4();
	auto colorrand = TGAColor(rand() % 255, rand() % 255, rand() % 255);

	int top = (int)ceil(std::max(v[0].y, std::max(v[1].y, v[2].y)));
	int bottom = (int)floor(std::min(v[0].y, std::min(v[1].y, v[2].y)));
	int left = (int)floor(std::min(v[0].x, std::min(v[1].x, v[2].x)));
	int right = (int)ceil(std::max(v[0].x, std::max(v[1].x, v[2].x)));

	for (int x = left; x <= right; x++)
	{
		for (int y = bottom; y <= top; y++)
		{
			if (insideTriangle(x, y, t.vert))
			{
				// (x,y) = alpha A + beta B + gamma C
				auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.vert);
				float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w; // compute point.z with w
				float Z = 1.0f / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w); // compute point.w
				zp *= Z;

				if (zp < zbuffer[get_index(x, y)])
				{
					// color
					auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
					// normal
					auto interpolated_normal = glm::normalize(interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1));
					// texcoords
					auto interpolated_texcoords = interpolate(alpha, beta, gamma, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], 1);
					// shadingcoords
					auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);

					float intensity = sature(glm::dot(interpolated_normal, light_dir));

					auto color = TGAColor(200 * intensity, 200 * intensity, 200 * intensity);

					// set zbuffer
					zbuffer[get_index(x, y)] = zp;

					//set color
					auto index = y * WIDTH + x;
					for (int i = 0; i < 4; i++)
					{
						pixels[index * 4 + i] = color[i] * intensity;
					}
				}
			}
		}
	}
}
