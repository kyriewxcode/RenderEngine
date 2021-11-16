#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;
Vec3f light_dir(0, 0, -1);

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color)
{
	bool steep = false;
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y))
	{
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}

	if (p0.x > p1.x)
	{
		std::swap(p0, p1);
	}

	for (int x = p0.x; x <= p1.x; x++)
	{
		float t = (x - p0.x) / (float)(p1.x - p0.x);
		int y = p0.y * (1.0 - t) + p1.y * t;
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
	}
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
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 0.01f)
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // 负坐标将会被忽略
}

void triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

	// 获取包围三角形的AABB
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
			bboxmin[j] = std::max(0.0f, std::min(bboxmin[j], pts[i][j]));
		}
	}

	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
				continue; // 只要有一个cross结果小于零，就在三角形外面

			P.z = 0;
			for (int i = 0; i < 3; ++i)
				P.z += pts[i][2] * bc_screen[i];

			// 深度缓冲比较
			if (zbuffer[int(P.x + P.y * width)] < P.z)
			{
				zbuffer[int(P.x + P.y * width)] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}
}

Vec3f world2screen(Vec3f v)
{
	return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

int main(int argc, char** argv)
{
	TGAImage image(width, height, TGAImage::RGB);
	float* zbuffer = new float[width * height];

	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("res/african_head/african_head.obj");

	Vec3f light_dir(0, 0, -1);
	for (int i = 0; i < model->nfaces(); ++i)
	{
		std::vector<int> face = model->face(i);
		Vec3f pts[3];
		for (int j = 0; j < 3; ++j)
			pts[j] = world2screen(model->vert(face[j]));

		Vec3f n = cross((pts[2] - pts[0]), (pts[1] - pts[0]));
		n.normalize();

		triangle(pts, zbuffer, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}

	image.flip_vertically();
	image.write_tga_file("output.tga");
	system("output.tga");
	delete model;
	return 0;
}


