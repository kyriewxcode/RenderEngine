#include "pipeline.h"
#include <GLFW\glfw3.h>

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

// = = = = == = == = = = ³É Ô± º¯ Êý = = = = = = = = = = = = = = = = =

static auto to_vec4(const glm::vec3& v3, float w)
{
	return glm::vec4(v3.x, v3.y, v3.z, w);
}

int Pipeline::get_index(int x, int y)
{
	return (height - y) * width + x;
}

glm::mat4 Pipeline::get_model_matrix(float angle)
{
	angle = angle * (float)MY_PI / 180.f;

	glm::mat4 rotation;
	rotation = {
		cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1
	};

	glm::mat4 scale;
	scale = {
		2.5, 0, 0, 0,
		0, 2.5, 0, 0,
		0, 0, 2.5, 0,
		0, 0, 0, 1
	};

	glm::mat4 translate;
	translate = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	return translate * rotation * scale;
}

glm::mat4 Pipeline::get_view_matrix(glm::vec3 eye_pos)
{
	glm::mat4 view(1);

	glm::mat4 translate;
	translate = {
		1, 0, 0, -eye_pos[0],
		0, 1, 0, -eye_pos[1],
		0, 0, 1, -eye_pos[2],
		0, 0, 0, 1
	};

	view = glm::transpose(translate) * view;

	return view;
}

glm::mat4 Pipeline::get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	glm::mat4 projection(1);

	float n = zNear;
	float f = zFar;
	float t = -n * tan((eye_fov / 2.0f) * DEG_TO_RAD);
	float r = aspect_ratio * t;
	float b = -t;
	float l = -r;

	glm::mat4 ortho(
		2 / (r - l), 0, 0, -(l + r) / 2,
		0, 2 / (t - b), 0, -(b + t) / 2,
		0, 0, 2 / (n - f), -(f + n) / 2,
		0, 0, 0, 1
	);

	glm::mat4 persp_to_ortho(
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -(n * f),
		0, 0, 1, 0
	);

	projection = glm::transpose(ortho) * glm::transpose(persp_to_ortho);


	return projection;
}

void Pipeline::render()
{
	zbuffer = std::vector<float>(width * height, std::numeric_limits<double>::max());
	pixels = new unsigned char[width * height * 4];
	// set triangle
	for (int i = 0; i < entity.model->nfaces(); i++)
	{
		Triangle* t = new Triangle();
		for (int j = 0; j < 3; j++)
		{
			glm::vec3 v = entity.model->vert(i, j);
			t->setVertex(j, glm::vec4(v.x, v.y, v.z, 1.0f));
			t->setNormal(j, entity.model->normal(i, j));
			t->setTexCoord(j, entity.model->uv(i, j));
		}
		TriangleList.push_back(t);
	}

	ModelView = get_model_matrix(0.0f);
	Viewport = get_view_matrix(eye_pos);
	Projection = get_projection_matrix(45.0f, 1.f, 0.1f, 50.f);

	float f1 = (50.f - 0.1f) / 2.0f;
	float f2 = (50.f + 0.1f) / 2.0f;
	glm::mat4 mvp = Projection * Viewport * ModelView;

	for (const auto& t : TriangleList)
	{
		Triangle newtri = *t;

		std::array<glm::vec4, 3> mm{
			(Viewport * ModelView * t->vert[0]),
			(Viewport * ModelView * t->vert[1]),
			(Viewport * ModelView * t->vert[2])
		};

		std::array<glm::vec3, 3> viewspace_pos;

		std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), [](auto& v) { return glm::vec3(v); });

		glm::vec4 v[] = {
				mvp * t->vert[0],
				mvp * t->vert[1],
				mvp * t->vert[2]
		};

		for (auto& vec : v)
		{
			vec.x /= vec.w;
			vec.y /= vec.w;
			vec.z /= vec.w;
		}

		glm::mat4 inv_trans = glm::transpose(glm::inverse(Viewport * ModelView));
		glm::vec4 n[] = {
				inv_trans * to_vec4(t->normal[0], 0.0f),
				inv_trans * to_vec4(t->normal[1], 0.0f),
				inv_trans * to_vec4(t->normal[2], 0.0f)
		};

		// viewport transformation
		for (auto& vert : v)
		{
			vert.x = 0.5f * (float)width * (vert.x + 1.0f);
			vert.y = 0.5f * (float)height * (vert.y + 1.0f);
			vert.z = vert.z * f1 + f2;

			if (vert.y < 0)
			{
				std::cout << "" << std::endl;
			}
		}

		for (int i = 0; i < 3; ++i)
		{
			// screen space position
			newtri.setVertex(i, v[i]);
		}

		for (int i = 0; i < 3; ++i)
		{
			// screen space normal
			newtri.setNormal(i, glm::vec3(n[i]));
		}

		newtri.setColor(0, 148, 121.0, 92.0);
		newtri.setColor(1, 148, 121.0, 92.0);
		newtri.setColor(2, 148, 121.0, 92.0);

		triangle(newtri, viewspace_pos);
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
				auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.vert);
				float Z = 1.0f / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
				float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w;
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

					auto color = TGAColor(250 * intensity, 250 * intensity, 250 * intensity);

					// set zbuffer
					zbuffer[get_index(x, y)] = zp;

					//set color
					auto index = y * width + x;
					pixels[index * 4 + 0] = 250 * intensity;
					pixels[index * 4 + 1] = 250 * intensity;
					pixels[index * 4 + 2] = 250 * intensity;
					pixels[index * 4 + 3] = 250 * intensity;
				}
			}
		}
	}

}

