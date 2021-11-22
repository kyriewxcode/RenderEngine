#include "main.h"

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "RenderEngine", NULL, NULL);

	std::vector<Entity> entities;
	const char* filename;

	filename = "obj/floor.obj";
	Entity plane(filename);
	plane.transform.position = glm::vec3(0, 0.f, 0);
	plane.transform.rotation = glm::vec3(0.f, 0., 0);
	plane.transform.scale = glm::vec3(6.f, 3.f, 3.f);
	entities.push_back(plane);

	filename = "obj/spot/spot_triangulated_good.obj";
	Entity cube(filename);
	cube.transform.position = glm::vec3(0, -1, 0);
	cube.transform.rotation = glm::vec3(0, 30.f + 180.f, 0);
	cube.transform.scale = glm::vec3(3.f, 3.f, 3.f);
	entities.push_back(cube);

	Pipeline pipeline(entities);
	pipeline.zbuffer = std::vector<float>(WIDTH * HEIGHT, std::numeric_limits<double>::max());
	pipeline.pixels = new unsigned char[WIDTH * HEIGHT * 4];

	glfwMakeContextCurrent(window);
	double lastTime = glfwGetTime();
	int frameCount = 0;

	pipeline.clear_color(glm::vec4(0, 51, 102, 255));

	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		frameCount++;
		glClear(GL_COLOR_BUFFER_BIT);

		pipeline.render();

		if (currentTime - lastTime >= 1.0)
		{
			std::string s;
			s = "RenderEngine | " + std::to_string(frameCount) + " FPS";
			glfwSetWindowTitle(window, s.c_str());
			lastTime = currentTime;
			frameCount = 0;
		}
		glDrawPixels(WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pipeline.pixels);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}