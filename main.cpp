#pragma GCC optimize(3,"Ofast","inline")
#include "main.h"

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(width, height, "RenderEngine", NULL, NULL);

	const char* floor = "obj/floor.obj";
	Entity plane(floor);

	const char* cube = "obj/cube.obj";
	Entity entity(cube);
	Shader shader;
	Pipeline pipeline(entity, shader);
	pipeline.zbuffer = std::vector<float>(width * height, std::numeric_limits<double>::max());
	pipeline.pixels = new unsigned char[width * height * 4];

	glfwMakeContextCurrent(window);
	double lastTime = glfwGetTime();
	int frameCount = 0;

	pipeline.clear_color(glm::vec4(149, 138, 153, 255));

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
		glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pipeline.pixels);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}