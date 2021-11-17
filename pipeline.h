#ifndef __PIPELINE_H__
#define __PIPELINE_H__
#include "tgaimage.h"
#include "mathtool.h"
#include "Shader.h"
#include "Entity.h"
#include "triangle.h"


class Pipeline
{
public:
	Shader shader;
	Entity entity;
	std::vector<Triangle*> TriangleList;

	TGAImage framebuffer;
	std::vector<float> zbuffer;

private:
	int width = 800;
	int height = 800;
	Vector3f eye_pos = { 0.f,0.f,10.0f };
	Vector3f light_dir = { 0.f,1.f,1.f };

	Matrix4f ModelView;
	Matrix4f Viewport;
	Matrix4f Projection;

public:
	Pipeline(Entity& e, Shader s) : entity(e), shader(s) {};
	TGAImage render();

private:
	int get_index(int x, int y);

	Matrix4f get_model_matrix(float rotation_angle);
	Matrix4f get_view_matrix(Eigen::Vector3f eye_pos);
	Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);
	void triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>& view_pos);
};

#endif // !__PIPELINE_H__
