#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__
#include "mathtool.h"

class Triangle
{
public:
	glm::vec4 vert[3]; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
	/*Per vertex values*/
	glm::vec3 color[3]; //color at each vertex;
	glm::vec2 tex_coords[3]; //texture u,v
	glm::vec3 normal[3]; //normal vector for each vertex

	Triangle();

	glm::vec4 a() const { return vert[0]; }
	glm::vec4 b() const { return vert[1]; }
	glm::vec4 c() const { return vert[2]; }

	void setVertex(int ind, glm::vec4 ver); /*set i-th vertex coordinates */
	void setNormal(int ind, glm::vec3 n); /*set i-th vertex normal vector*/
	void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/

	void setNormals(const std::array<glm::vec3, 3>& normals);
	void setColors(const std::array<glm::vec3, 3>& colors);
	void setTexCoord(int ind, glm::vec2 uv); /*set i-th vertex texture coordinate*/
	std::array<glm::vec4, 3> toVector4() const;
};


#endif // !__TRIANGLE_H__
