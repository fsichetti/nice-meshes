#ifndef PLANESAMPLING_H
#define PLANESAMPLING_H

#include "Mesh.hpp"

struct PlaneSampling {
    Mesh::vArray verts;     // vertices in 2D
    Mesh::fArray faces;
    PlaneSampling(std::string path);
	// Get vertex and face number
	const inline unsigned int vertNum() const { return verts.size()/2; }
	const inline unsigned int faceNum() const { return faces.size()/3; }

	const inline GLfloat cAttrib(unsigned int vertexId,
		unsigned int attribOffset) const {
		return verts[2 * vertexId + attribOffset];
	}
	const inline GLuint cFacei(unsigned int faceId, unsigned int n) const {
		return faces[3 * faceId + n];
	}
};

#endif