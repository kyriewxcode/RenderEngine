#ifndef __PIPELINE_H__
#define __PIPELINE_H__
#include "math.h"
#include "Entity.h"
#include "triangle.h"
#include "Camera.h"

enum class Face
{
	Back = 0,
	Front = 1
};

class Pipeline
{
public:
	Camera camera;
	std::vector<Entity> entities;
	std::vector<float> zbuffer;
	unsigned char* pixels;

private:
	glm::mat4x4 ModelView;
	glm::mat4x4 Viewport;
	glm::mat4x4 Projection;

	const glm::vec3 lightPos = { 1.f,1.f,1.f };
	const glm::vec4 lightColor = { 250, 250, 250 ,255 };

public:
	Pipeline(std::vector<Entity> e) : entities(e)
	{
		zbuffer = std::vector<float>((WIDTH + 1) * (HEIGHT + 1), std::numeric_limits<double>::max());
		pixels = new unsigned char[WIDTH * HEIGHT * 4];

		Viewport = glm::lookAt(
			camera.transform.position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		Projection = glm::perspective(camera.fov, camera.width / camera.height, camera.zNear, camera.zFar);
	};
	void render();
	void clear_color(glm::vec4 color);

private:
	int get_index(int x, int y);

	glm::mat4 get_model_matrix(Transform transform);
	bool ClipSpaceCull(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3);
	bool FaceCull(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& v3, Face face);
	void triangle(const Triangle& t, const std::array<glm::vec3, 3>& view_pos, const glm::vec3 lightDir);
};

#endif // !__PIPELINE_H__
