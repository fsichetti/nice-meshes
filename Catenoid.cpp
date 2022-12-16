#include "Catenoid.hpp"

Catenoid::Catenoid(
        unsigned int samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true) {

    // Sampling in rotational direction (uniform)
    const unsigned int uSamples = samples;
    const double uStep = 1.0 / static_cast<double>(uSamples);
    const double halfHeight = rInner * acosh(rOuter / rInner);
    const double height = 2 * halfHeight;

    // Pre-computation for sampling in vertical direction
    const double temp = TWOPI * SQRT3_2 * uStep * rInner;
    const unsigned int vSamples = std::ceil(height / temp);
    const double vStep = 1.0 / static_cast<double>(vSamples);
    // rescale to [0, 1]
    const unsigned int uvSamples = uSamples * vSamples;

    // Create object
    name = "RegularCatenoid";
    reserveSpace(uvSamples, 2*uvSamples);        


    for (unsigned int v = 0; v < vSamples; ++v) {
        for (unsigned int u = 0; u < uSamples; ++u) {
            // Place vertices
            const double uu = (u - (v%2)*.5) * uStep;
            const double vv = v * vStep;
            const double upi = uu * TWOPI, vs = (vv-.5)*height;
            const double cat = rInner * cosh(vs / rInner);
            GLdouble x = cat * cos(upi);
            GLdouble y = cat * sin(upi);
            GLdouble z = vs;
            addVertex(x, y, z);

            // Write parametric coordinates
            const unsigned int index = u + v*uSamples;
            attrib(index, Attribute::U) = uu;
            attrib(index, Attribute::V) = vv;

            // Add faces
            if (v != vSamples-1) {    // Open at the ends
                unsigned int us = (u+uSamples-v%2)%uSamples;
                GLuint a = index;
                GLuint b = (u+1)%uSamples+v*uSamples;
                GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
                GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

                addFace(a, b, c);
                addFace(a, c, d);
            }
        }
    }
}