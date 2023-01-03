#include "Sphere.hpp"

Sphere::Sphere(unsigned int subdiv, double radius) : Mesh(true, true, true) {
    name = "sphere";

    const unsigned int faceNumber = 20 * pow(4,subdiv);
    const unsigned int vertexNumber = 2 + faceNumber / 2;
    reserveSpace(vertexNumber, faceNumber);
    initIcosahedron();
    // Subdivision
    for (unsigned int i = 0; i < subdiv; ++i) {
        refine();
    }
    // Normalization, normals, UV
    const unsigned int s = vertexNumber;
    for (unsigned int i = 0; i < s; ++i) {
        double x = cAttrib(i, Attribute::X);
        double y = cAttrib(i, Attribute::Y);
        double z = cAttrib(i, Attribute::Z);
        const double nrm = 1.0 / sqrt(x*x + y*y + z*z);
        // Normalized coordinates
        x *= nrm;
        y *= nrm;
        z *= nrm;
        attrib(i, Attribute::X) = x * radius;
        attrib(i, Attribute::Y) = y * radius;
        attrib(i, Attribute::Z) = z * radius;
        attrib(i, Attribute::NX) = x;
        attrib(i, Attribute::NY) = y;
        attrib(i, Attribute::NZ) = z;
        attrib(i, Attribute::U) = acos(z) / M_PI;
        const double f = sqrt(x*x + y*y);
        attrib(i, Attribute::V) = (f == 0) ? 0 : (
            glm::sign(y) * acos(x / f) / TWOPI + .5
        );
        const float h = 1 / radius;
        attrib(i, Attribute::H) = h;
        attrib(i, Attribute::K) = h / radius;
    }
    computeNormals(true);
}

void Sphere::initIcosahedron() {
    const double& p = PSI;
    addVertex(-1,  p,  0);
    addVertex( 1,  p,  0);
    addVertex(-1, -p,  0);
    addVertex( 1, -p,  0);
    addVertex( 0, -1,  p);
    addVertex( 0,  1,  p);
    addVertex( 0, -1, -p);
    addVertex( 0,  1, -p);
    addVertex( p,  0, -1);
    addVertex( p,  0,  1);
    addVertex(-p,  0, -1);
    addVertex(-p,  0,  1);
    addFace(0, 11, 5);
    addFace(0, 5, 1);
    addFace(0, 1, 7);
    addFace(0, 7, 10);
    addFace(0, 10, 11);
    addFace(1, 5, 9);
    addFace(5, 11, 4);
    addFace(11, 10, 2);
    addFace(10, 7, 6);
    addFace(7, 1, 8);
    addFace(3, 9, 4);
    addFace(3, 4, 2);
    addFace(3, 2, 6);
    addFace(3, 6, 8);
    addFace(3, 8, 9);
    addFace(4, 9, 5);
    addFace(2, 4, 11);
    addFace(6, 2, 10);
    addFace(8, 6, 7);
    addFace(9, 8, 1);
}