#include "pipeline.h"

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v)
{
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}

static bool insideTriangle(int x, int y, const Vector4f* _v)
{
	Vector3f v[3];
	for (int i = 0; i < 3; i++)
		v[i] = { _v[i].x(),_v[i].y(), 1.0 };
	Vector3f f0, f1, f2;
	f0 = v[1].cross(v[0]);
	f1 = v[2].cross(v[1]);
	f2 = v[0].cross(v[2]);
	Vector3f p(x, y, 1.);
	if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) && (p.dot(f2) * f2.dot(v[1]) > 0))
		return true;
	return false;
}

static Vector2f interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3, float weight)
{
	auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
	auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

	u /= weight;
	v /= weight;

	return Vector2f(u, v);
}

static Vector3f interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3, float weight)
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

static auto to_vec4(const Vector3f& v3, float w)
{
	return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

int Pipeline::get_index(int x, int y)
{
	return (height - y) * width + x;
}

Matrix4f Pipeline::get_model_matrix(float angle)
{
	angle = angle * MY_PI / 180.f;

	Matrix4f rotation;
	rotation <<
		cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1;

	Matrix4f scale;
	scale <<
		2.5, 0, 0, 0,
		0, 2.5, 0, 0,
		0, 0, 2.5, 0,
		0, 0, 0, 1;

	Matrix4f translate;
	translate <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	return translate * rotation * scale;
}

Matrix4f Pipeline::get_view_matrix(Vector3f eye_pos)
{
	Matrix4f view = Matrix4f::Identity();

	Matrix4f translate;
	translate <<
		1, 0, 0, -eye_pos[0],
		0, 1, 0, -eye_pos[1],
		0, 0, 1, -eye_pos[2],
		0, 0, 0, 1;

	view = translate * view;

	return view;
}

Matrix4f Pipeline::get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	Matrix4f projection = Matrix4f::Identity();
	Matrix4f ortho = Matrix4f::Identity();
	Matrix4f persp_to_ortho = Matrix4f::Identity();

	float n = zNear;
	float f = zFar;
	float t = -n * tan((eye_fov / 2.0) * DEG_TO_RAD);
	float r = aspect_ratio * t;
	float b = -t;
	float l = -r;

	ortho <<
		2 / (r - l), 0, 0, -(l + r) / 2,
		0, 2 / (t - b), 0, -(b + t) / 2,
		0, 0, 2 / (n - f), -(f + n) / 2,
		0, 0, 0, 1;

	persp_to_ortho <<
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -(n * f),
		0, 0, 1, 0;

	projection = ortho * persp_to_ortho;


	return projection;
}

TGAImage Pipeline::render()
{
	framebuffer = TGAImage(width, height, TGAImage::RGB);
	zbuffer = std::vector<float>(width * height, std::numeric_limits<double>::max());

	// set triangle
	for (int i = 0; i < entity.model->nfaces(); i++)
	{
		Triangle* t = new Triangle();
		for (int j = 0; j < 3; j++)
		{
			Vector3f v = entity.model->vert(i, j);
			t->setVertex(j, Vector4f(v.x(), v.y(), v.z(), 1.0f));
			t->setNormal(j, entity.model->normal(i, j));
			t->setTexCoord(j, entity.model->uv(i, j));
		}
		TriangleList.push_back(t);
	}

	ModelView = get_model_matrix(0.0f);
	Viewport = get_view_matrix(eye_pos);
	Projection = get_projection_matrix(45.0, 1, 0.1, 50);

	float f1 = (50 - 0.1) / 2.0;
	float f2 = (50 + 0.1) / 2.0;
	Matrix4f mvp = Projection * Viewport * ModelView;

	for (const auto& t : TriangleList)
	{
		Triangle newtri = *t;

		std::array<Vector4f, 3> mm{
			(Viewport * ModelView * t->vert[0]),
			(Viewport * ModelView * t->vert[1]),
			(Viewport * ModelView * t->vert[2])
		};

		std::array<Vector3f, 3> viewspace_pos;

		std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), [](auto& v) { return v.template head<3>(); });

		Vector4f v[] = {
				mvp * t->vert[0],
				mvp * t->vert[1],
				mvp * t->vert[2]
		};

		for (auto& vec : v)
		{
			vec.x() /= vec.w();
			vec.y() /= vec.w();
			vec.z() /= vec.w();
		}

		Matrix4f inv_trans = (Viewport * ModelView).inverse().transpose();
		Vector4f n[] = {
				inv_trans * to_vec4(t->normal[0], 0.0f),
				inv_trans * to_vec4(t->normal[1], 0.0f),
				inv_trans * to_vec4(t->normal[2], 0.0f)
		};

		// viewport transformation
		for (auto& vert : v)
		{
			vert.x() = 0.5 * width * (vert.x() + 1.0);
			vert.y() = 0.5 * height * (vert.y() + 1.0);
			vert.z() = vert.z() * f1 + f2;
		}

		for (int i = 0; i < 3; ++i)
		{
			// screen space position
			newtri.setVertex(i, v[i]);
		}

		for (int i = 0; i < 3; ++i)
		{
			// screen space normal
			newtri.setNormal(i, n[i].head<3>());
		}

		newtri.setColor(0, 148, 121.0, 92.0);
		newtri.setColor(1, 148, 121.0, 92.0);
		newtri.setColor(2, 148, 121.0, 92.0);

		triangle(newtri, viewspace_pos);
	}

	return framebuffer;
}

void Pipeline::triangle(const Triangle& t, const std::array<Vector3f, 3>& view_pos)
{
	auto v = t.toVector4();
	auto colorrand = TGAColor(rand() % 255, rand() % 255, rand() % 255);

	int top = ceil(std::max(v[0].y(), std::max(v[1].y(), v[2].y())));
	int bottom = floor(std::min(v[0].y(), std::min(v[1].y(), v[2].y())));
	int left = floor(std::min(v[0].x(), std::min(v[1].x(), v[2].x())));
	int right = ceil(std::max(v[0].x(), std::max(v[1].x(), v[2].x())));

	for (int x = left; x <= right; x++)
	{
		for (int y = bottom; y <= top; y++)
		{
			if (insideTriangle(x, y, t.vert))
			{
				auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.vert);
				float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
				float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
				zp *= Z;

				if (zp < zbuffer[get_index(x, y)])
				{
					// color
					auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
					// normal
					auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1).normalized();
					// texcoords
					auto interpolated_texcoords = interpolate(alpha, beta, gamma, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], 1);
					// shadingcoords
					auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);

					float intensity = sature(interpolated_normal.dot(light_dir));

					auto color = TGAColor(250 * intensity, 250 * intensity, 250 * intensity);
					
					// set zbuffer
					zbuffer[get_index(x, y)] = zp;

					//set color
					framebuffer.set(x, y, color);
				}
			}
		}
	}

}

