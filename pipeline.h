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

	glm::vec3 eye_pos = { 0.f,0.f,3.f };
	glm::vec3 light_dir = { 0.f,1.f,1.f };

	glm::mat4x4 ModelView;
	glm::mat4x4 Viewport;
	glm::mat4x4 Projection;

public:
	Pipeline(Entity& e, Shader s) : entity(e), shader(s)
	{
		zbuffer = std::vector<float>(width * height + 1, std::numeric_limits<double>::max());
		pixels = new unsigned char[width * height * 4];
	};
	void render();
	void clear_color(glm::vec4 color);

private:
	int get_index(int x, int y);

	glm::mat4x4 get_model_matrix(float rotation_angle);
	glm::mat4x4 get_view_matrix(glm::vec3 eye_pos);
	glm::mat4x4 get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);
	void set_triangle_list(Entity entity);
	void triangle(const Triangle& t, const std::array<glm::vec3, 3>& view_pos);
};

#endif // !__PIPELINE_H__
