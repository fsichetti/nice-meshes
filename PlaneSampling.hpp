#ifndef PLANESAMPLING_H
#define PLANESAMPLING_H

#include "Mesh.hpp"

class PlaneSampling {
	public:
    Mesh::vArray verts;     // vertices in 2D
    Mesh::fArray faces;
    PlaneSampling(std::string path);
	// Get vertex and face number
	const inline uint vertNum() const { return verts.size()/2; }
	const inline uint faceNum() const { return faces.size()/3; }

	const inline double cAttrib(uint vertexId,
		uint attribOffset) const {
		return verts[2 * vertexId + attribOffset];
	}
	const inline uint cFacei(uint faceId, uint n) const {
		return faces[3 * faceId + n];
	}
};

#endif