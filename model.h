#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include "tgaimage.h"
#include "mathtool.h"

class Model
{
private:
	std::vector<Vector3f> verts_;     // array of vertices
	std::vector<Vector2f> uv_;        // array of tex coords
	std::vector<Vector3f> norms_;     // array of normal vectors
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
	Vector3f normal(const int iface, const int nthvert) const;  // per triangle corner normal vertex
	Vector3f normal(const Vector2f& uv) const;                      // fetch the normal vector from the normal map texture
	Vector3f vert(const int i) const;
	Vector3f vert(const int iface, const int nthvert) const;
	Vector2f uv(const int iface, const int nthvert) const;
	TGAColor diffuse(const Vector2f& uv) const;
	double specular(const Vector2f& uv) const;
};
#endif //__MODEL_H__

