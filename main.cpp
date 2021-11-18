#include "main.h"

int main(int argc, char** argv)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "RenderEngine", NULL, NULL);

	Model* model = NULL;
	if (argc == 2)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head/african_head.obj");
	glm::vec3 vec3fzero = glm::vec3(0.0f, 0.0f, 0.0f);
	Transform trans(vec3fzero, vec3fzero, vec3fzero);
	Entity entity(trans, model);
	Shader shader;
	Pipeline pipeline(entity, shader);
	

	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window))
	{
		pipeline.render();

		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawPixels(800, 600, GL_RGBA, GL_UNSIGNED_BYTE, pipeline.pixels);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();


	return 0;
}
