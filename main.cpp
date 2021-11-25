#include "Main.h"
#include "Screen.h"
#include "Pipeline.h"

extern const float width{ 800 }, height{ 600 };

typedef struct { float m[4][4]; } matrix_t;

extern const float PI = 3.14159265f;

int main(void)
{
	const char* title = "Render Engine";

	Screen screen;

	if (screen.init(width, height, title))
		return -1;

	Device device(screen.m_fb, width, height);

	Pipeline pipeline(&device);

	Entity box("obj/cube/cube.obj");
	box.transform.eulerAngles = glm::vec3(0, 0, 0);
	pipeline.addEntity(&box);

	Entity plane("obj/floor.obj");
	plane.transform.scale = glm::vec3(3);
	pipeline.addEntity(&plane);

	clock_t t1 = clock();
	int fpsCount = 0;
	while (screen.m_exit == 0 && screen.m_keys[VK_ESCAPE] == 0)
	{
		box.update();

		float deltaTime = (clock() - t1) * 1.0 / CLOCKS_PER_SEC;
		if (deltaTime >= 1.f)
		{
			std::cout << fpsCount << " FPS" << std::endl;
			fpsCount = 0;
			t1 = clock();
		}
		screen.dispatch();
		device.clear();
		pipeline.draw();
		screen.update();
		fpsCount++;
	}

	return 0;
}



//#include "main.h"
//
//int main(int argc, char** argv)
//{
//	glfwInit();
//	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "RenderEngine", NULL, NULL);
//
//	std::vector<Entity> entities;
//	const char* filename;
//
//	filename = "obj/floor.obj";
//	Entity plane(filename);
//	plane.transform.position = glm::vec3(0, -2.f, 0);
//	plane.transform.rotation = glm::vec3(0, 0, 0);
//	plane.transform.scale = glm::vec3(6.f, 1.f, 2.f);
//	entities.push_back(plane);
//
//	filename = "obj/spot/spot_triangulated_good.obj";
//	Entity cube(filename);
//	cube.transform.position = glm::vec3(0, -1, 0);
//	cube.transform.rotation = glm::vec3(0, 30.f + 180.f, 0);
//	cube.transform.scale = glm::vec3(3.f, 3.f, 3.f);
//	entities.push_back(cube);
//
//	Pipeline pipeline(entities);
//	pipeline.zbuffer = std::vector<float>(WIDTH * HEIGHT, std::numeric_limits<double>::max());
//	pipeline.pixels = new unsigned char[WIDTH * HEIGHT * 4];
//
//	glfwMakeContextCurrent(window);
//	double lastTime = glfwGetTime();
//	int frameCount = 0;
//
//	pipeline.clear_color(glm::vec4(0, 51, 102, 255));
//
//	while (!glfwWindowShouldClose(window))
//	{
//		double currentTime = glfwGetTime();
//		frameCount++;
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		pipeline.render();
//
//		if (currentTime - lastTime >= 1.0)
//		{
//			std::string s;
//			s = "RenderEngine | " + std::to_string(frameCount) + " FPS";
//			glfwSetWindowTitle(window, s.c_str());
//			lastTime = currentTime;
//			frameCount = 0;
//		}
//		glDrawPixels(WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pipeline.pixels);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	glfwTerminate();
//	return 0;
//}