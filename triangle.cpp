#include "triangle.h"
#include <algorithm>
#include <array>

Triangle::Triangle()
{
	vert[0] = { 0, 0, 0, 1 };
	vert[1] = { 0, 0, 0, 1 };
	vert[2] = { 0, 0, 0, 1 };

	color[0] = { 0.0, 0.0, 0.0 };
	color[1] = { 0.0, 0.0, 0.0 };
	color[2] = { 0.0, 0.0, 0.0 };

	tex_coords[0] = { 0.0, 0.0 };
	tex_coords[1] = { 0.0, 0.0 };
	tex_coords[2] = { 0.0, 0.0 };
}

void Triangle::setVertex(int ind, glm::vec4 ver)
{
	vert[ind] = ver;
}
void Triangle::setNormal(int ind, glm::vec3 n)
{
	normal[ind] = n;
}
void Triangle::setColor(int ind, float r, float g, float b)
{
	if ((r < 0.0) || (r > 255.) ||
		(g < 0.0) || (g > 255.) ||
		(b < 0.0) || (b > 255.))
	{
		fprintf(stderr, "ERROR! Invalid color values");
		fflush(stderr);
		exit(-1);
	}

	color[ind] = glm::vec3((float)r / 255., (float)g / 255., (float)b / 255.);
	return;
}
void Triangle::setTexCoord(int ind, glm::vec2 uv)
{
	tex_coords[ind] = uv;
}

std::array<glm::vec4, 3> Triangle::toVector4() const
{
	std::array<glm::vec4, 3> res;
	std::transform(std::begin(vert), std::end(vert), res.begin(), [](auto& vec) { return glm::vec4(vec.x, vec.y, vec.z, 1.f); });
	return res;
}

void Triangle::setNormals(const std::array<glm::vec3, 3>& normals)
{
	normal[0] = normals[0];
	normal[1] = normals[1];
	normal[2] = normals[2];
}

void Triangle::setColors(const std::array<glm::vec3, 3>& colors)
{
	auto first_color = colors[0];
	setColor(0, colors[0][0], colors[0][1], colors[0][2]);
	setColor(1, colors[1][0], colors[1][1], colors[1][2]);
	setColor(2, colors[2][0], colors[2][1], colors[2][2]);
}