#include "model.h"
#include <iostream>
#include "Entity.h"
#include "Shader.h"
#include "pipeline.h"


int main(int argc, char** argv)
{
	Model* model = NULL;
	if (argc == 2)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head/african_head.obj");

	Vector3f vec3fzero = Vector3f(0.0f, 0.0f, 0.0f);
	Transform trans(vec3fzero, vec3fzero, vec3fzero);
	Entity entity(trans, model);
	Shader shader;
	Pipeline pipeline(entity, shader);
	pipeline.render().write_tga_file("output.tga");

	system("output.tga");
	return 0;
}
