#pragma once
#ifndef __MESHLET_H_
#define __MESHLET_H_

#include "include/meshoptimizer/meshoptimizer.h"
#include <vector>

struct Mesh
{
	int verticesSize;
	std::vector<unsigned int> indices;
};

void GetMeshletTest(const Mesh& mesh, std::vector<meshopt_Meshlet>& meshlets, std::vector<unsigned int>& meshlet_vertices, std::vector<unsigned int>& triangles)
{
	const size_t max_vertices = 64;
	const size_t max_triangles = 124;

	size_t max_meshlets = meshopt_buildMeshletsBound(mesh.indices.size(), max_vertices, max_triangles);
	meshlets.resize(max_meshlets);
	meshlet_vertices.resize(max_meshlets * max_vertices);
	std::vector<unsigned char> meshlet_triangles(max_meshlets * max_triangles * 3);

	meshlets.resize(meshopt_buildMeshletsScan(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), mesh.verticesSize, max_vertices, max_triangles));

	std::vector<unsigned int> uintTriangle(meshlet_triangles.begin(), meshlet_triangles.end());

	if (meshlets.size())
	{
		const meshopt_Meshlet& last = meshlets.back();
		meshlet_vertices.resize(last.vertex_offset + last.vertex_count);
		uintTriangle.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));
	}
	triangles = uintTriangle;
}
#endif // !__MESHLET_H_

