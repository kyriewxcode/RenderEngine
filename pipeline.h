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

	std::vector<float> zbuffer;
	unsigned char* pixels;

private:
	int width = 800;
	int height = 600;
	glm::vec3 eye_pos = { 0.f,0.f,10.0f };
	glm::vec3 light_dir = { 0.f,1.f,1.f };

	glm::mat4x4 ModelView;
	glm::mat4x4 Viewport;
	glm::mat4x4 Projection;

public:
	Pipeline(Entity& e, Shader s) : entity(e), shader(s) {};
	void render();

private:
	int get_index(int x, int y);

	glm::mat4x4 get_model_matrix(float rotation_angle);
	glm::mat4x4 get_view_matrix(glm::vec3 eye_pos);
	glm::mat4x4 get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);
	void triangle(const Triangle& t, const std::array<glm::vec3, 3>& view_pos);
};

#endif // !__PIPELINE_H__
