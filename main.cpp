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

	//Entity plane("obj/floor.obj");
	//plane.transform.scale = glm::vec3(3);
	//pipeline.addEntity(&plane);

	Entity bunny("obj/bunny/bunny.obj");
	bunny.transform.position = glm::vec3(0, 0, -7);
	bunny.transform.eulerAngles = glm::vec3(0, 180, 0);
	pipeline.addEntity(&bunny);

	clock_t t1 = clock();
	int fpsCount = 0;
	while (screen.m_exit == 0 && screen.m_keys[VK_ESCAPE] == 0)
	{
		bunny.update();

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