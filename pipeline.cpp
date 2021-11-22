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

// = = = = == = == = = = ³É Ô± º¯ Êý = = = = = = = = = = = = = = = = =

static auto to_vec4(const glm::vec3& v3, float w)
{
	return glm::vec4(v3.x, v3.y, v3.z, w);
}

int Pipeline::get_index(int x, int y)
{
	return (height - y) * width + x;
}

glm::mat4 Pipeline::get_model_matrix(Transform transform)
{
	// Rotation
	float angleX = glm::radians(transform.rotation.x);
	glm::mat4 rotationX;
	rotationX = {
		1, 0, 0, 0,
		0, cos(angleX),-sin(angleX), 0,
		0, sin(angleX), cos(angleX), 0,
		0, 0, 0, 1
	};

	float angleY = glm::radians(transform.rotation.y);
	glm::mat4 rotationY;
	rotationY = {
		cos(angleY), 0, sin(angleY), 0,
		0, 1, 0, 0,
		-sin(angleY), 0, cos(angleY), 0,
		0, 0, 0, 1
	};

	float angleZ = glm::radians(transform.rotation.z);
	glm::mat4 rotationZ;
	rotationZ = {
		cos(angleZ), -sin(angleZ), 0, 0,
		sin(angleZ), cos(angleZ), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	glm::mat4 rotationMatrix = rotationX * rotationY * rotationZ;

	// Scale
	glm::vec3 scale = transform.scale;
	glm::mat4 scaleMatrix;
	scaleMatrix = {
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	};

	// Position
	glm::vec3 pos = transform.position;
	glm::mat4 translateMatrix;
	translateMatrix = {
		1, 0, 0, pos.x,
		0, 1, 0, pos.y,
		0, 0, 1, pos.z,
		0, 0, 0, 1
	};

	return glm::transpose(translateMatrix) * glm::transpose(rotationMatrix) * glm::transpose(scaleMatrix);
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
	float t = n * tan(glm::radians(eye_fov / 2.0f));
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

void Pipeline::clear_color(glm::vec4 color)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			auto index = y * width + x;
			pixels[index * 4 + 0] = color.r;
			pixels[index * 4 + 1] = color.g;
			pixels[index * 4 + 2] = color.b;
			pixels[index * 4 + 3] = color.a;
		}
	}
}

void Pipeline::render()
{
	// set triangle
	set_triangle_list(entity);

	// get mvp matrix
	ModelView = get_model_matrix(entity.transform);
	Viewport = get_view_matrix(eye_pos);
	Projection = get_projection_matrix(60.0f, width / height, -0.3f, -1000.f);
	glm::mat4 mvp = Projection * Viewport * ModelView;

	// rasterization
	for (const auto& t : TriangleList)
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

		for (auto& v : vertex)
		{
			v.x /= v.w;
			v.y /= v.w;
			if (v.x > 1.f || v.x < -1.f || v.y>1.f || v.y < -1.f)
			{
				std::cout << "out screen" << std::endl;
			}
			v.z /= v.w;
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
			v.x = 0.5f * (float)width * (v.x + 1.0f);
			v.y = 0.5f * (float)height * (v.y + 1.0f);
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
	TriangleList.clear();
}

void Pipeline::set_triangle_list(Entity entity)
{
	// loop over shapes
	for (size_t s = 0; s < entity.shapes.size(); s++)
	{
		size_t index_offset = 0;
		// loop over faces(polygon)
		for (size_t f = 0; f < entity.shapes[s].mesh.num_face_vertices.size(); f++)
		{
			Triangle* t = new Triangle();
			size_t fv = size_t(entity.shapes[s].mesh.num_face_vertices[f]);
			//loop over vertices in the face
			for (size_t v = 0; v < fv; v++)
			{
				// vertex data
				tinyobj::index_t idx = entity.shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = entity.attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = entity.attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = entity.attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				t->setVertex(v, glm::vec4(vx, vy, vz, 1));

				// normal data
				if (idx.normal_index >= 0)
				{
					tinyobj::real_t nx = entity.attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = entity.attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = entity.attrib.normals[3 * size_t(idx.normal_index) + 2];
					t->setNormal(v, glm::vec3(nx, ny, nz));
				}

				// texcoord data
				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t tx = entity.attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = entity.attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					t->setTexCoord(v, glm::vec2(tx, ty));
				}

				// vertex colors
				tinyobj::real_t red = entity.attrib.colors[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t green = entity.attrib.colors[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t blue = entity.attrib.colors[3 * size_t(idx.vertex_index) + 2];
				t->setColor(v, red, green, blue);
			}
			TriangleList.push_back(t);
			index_offset += fv;
			entity.shapes[s].mesh.material_ids[f];
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

				if (get_index(x, y) > width * height)
					continue;

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
					auto index = y * width + x;
					for (int i = 0; i < 4; i++)
					{
						pixels[index * 4 + i] = color[i] * intensity;
					}
				}
			}
		}
	}
}
