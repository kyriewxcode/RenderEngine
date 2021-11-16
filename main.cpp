#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

Model* model = NULL;

const int width = 800;
const int height = 800;
const int depth = 255;

Vec3f light_dir(0, 0, -1);
Vec3f camera(0, 0, 3);

Matrix m2v(Vec3f v)
{
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}

Vec3f m2v(Matrix m)
{
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Matrix viewport(int x, int y, int w, int h)
{
	Matrix m = Matrix::identity(4);
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
	Vec3f s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i]; // CA
		s[i][1] = B[i] - A[i]; // BA
		s[i][2] = A[i] - P[i]; // AP
	}
	Vec3f u = s[0] ^ s[1];
	if (std::abs(u[2]) > 0.01f)
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // 负坐标将会被忽略
}

void triangle(Vec3f* positionCS, Vec2i* uv, float* zbuffer, TGAImage& image, float intensity)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

	// 获取包围三角形的AABB
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			bboxmin[j] = std::max(0.0f, std::min(bboxmin[j], positionCS[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], positionCS[i][j]));
		}
	}

	Vec3f P;

	for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
	{
		for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
		{
			Vec3f bc_screen = barycentric(positionCS[0], positionCS[1], positionCS[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
				continue; // 只要有一个cross结果小于零，就在三角形外面

			auto ColorA = model->diffuse(uv[0]);
			auto ColorB = model->diffuse(uv[1]);
			auto ColorC = model->diffuse(uv[2]);

			TGAColor color = TGAColor(
				ColorA.r * bc_screen.x + ColorB.r * bc_screen.y + ColorC.r * bc_screen.z,
				ColorA.g * bc_screen.x + ColorB.g * bc_screen.y + ColorC.g * bc_screen.z,
				ColorA.b * bc_screen.x + ColorB.b * bc_screen.y + ColorC.b * bc_screen.z,
				255);

			P.z = 0;
			for (int i = 0; i < 3; ++i)
				P.z += positionCS[i][2] * bc_screen[i];

			// 深度缓冲比较
			if (zbuffer[int(P.x + P.y * width)] < P.z)
			{
				zbuffer[int(P.x + P.y * width)] = P.z;
				image.set(P.x, P.y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity));
			}
		}
	}
}

int main(int argc, char** argv)
{
	TGAImage image(width, height, TGAImage::RGB);
	float* zbuffer = new float[width * height];

	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head/african_head.obj");

	Matrix Projection = Matrix::identity(4);
	Matrix ViewPort = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	Projection[3][2] = -1.f / camera.z;

	Vec3f light_dir(0, 0, -1);
	for (int i = 0; i < model->nfaces(); ++i)
	{
		std::cout << i << std::endl;
		std::vector<int> face = model->face(i);
		Vec3f screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; ++j)
		{
			Vec3f v = model->vert(face[j]);
			Matrix ModelVertex = m2v(v);
			screen_coords[j] = (Vec3i)m2v(ViewPort * Projection * ModelVertex);
			world_coords[j] = v;
		}

		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0)
		{
			Vec2i uv[3];
			for (int k = 0; k < 3; k++)
			{
				uv[k] = model->uv(i, k);
			}
			triangle(screen_coords, uv, zbuffer, image, intensity);
		}
	}

	image.flip_vertically();
	image.write_tga_file("output.tga");
	system("output.tga");
	delete model;
	return 0;
}


