#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include "tgaimage.h"
#include "mathtool.h"

class Model
{
	
private:
	std::vector<glm::vec3> verts_;     // array of vertices
	std::vector<glm::vec2> uv_;        // array of tex coords
	std::vector<glm::vec3> norms_;     // array of normal vectors
	std::vector<int> facet_vrt_;
	std::vector<int> facet_tex_;  // indices in the above arrays per triangle
	std::vector<int> facet_nrm_;
	TGAImage diffusemap_;         // diffuse color texture
	TGAImage normalmap_;          // normal map texture
	TGAImage specularmap_;        // specular map texture
	void load_texture(const std::string filename, const std::string suffix, TGAImage& img);
public:
	Model(const std::string filename);
	int nverts() const;
	int nfaces() const;
	glm::vec3 normal(const int iface, const int nthvert) const;  // per triangle corner normal vertex
	glm::vec3 normal(const glm::vec2& uv) const;                      // fetch the normal vector from the normal map texture
	glm::vec3 vert(const int i) const;
	glm::vec3 vert(const int iface, const int nthvert) const;
	glm::vec2 uv(const int iface, const int nthvert) const;
	TGAColor diffuse(const glm::vec2& uv) const;
	double specular(const glm::vec2& uv) const;
};
#endif //__MODEL_H__

