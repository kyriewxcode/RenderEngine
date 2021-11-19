#include "main.h"

int main(int argc, char** argv)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(width, height, "RenderEngine", NULL, NULL);

	Model* model = NULL;
	if (argc == 2)
		model = new Model(argv[1]);
	else
		model = new Model("obj/diablo3_pose/diablo3_pose.obj");
	glm::vec3 vec3fzero = glm::vec3(0.0f, 0.0f, 0.0f);
	Transform trans(vec3fzero, vec3fzero, vec3fzero);
	Entity entity(trans, model);
	Shader shader;
	Pipeline pipeline(entity, shader);

	glfwMakeContextCurrent(window);

	double lastTime = glfwGetTime();
	int frameCount = 0;

	pipeline.zbuffer = std::vector<float>(width * height, std::numeric_limits<double>::max());
	pipeline.pixels = new unsigned char[width * height * 4];

	while (!glfwWindowShouldClose(window))
	{
		pipeline.render();

		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - lastTime >= 1.0)
		{
			std::string s;
			s = "RenderEngine | " + std::to_string(frameCount) + " FPS";
			glfwSetWindowTitle(window, s.c_str());
			lastTime = currentTime;
			frameCount = 0;
		}

		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pipeline.pixels);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();


	return 0;
}
