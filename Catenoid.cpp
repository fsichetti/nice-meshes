#include "Catenoid.hpp"

Catenoid::Catenoid(
        unsigned int samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true) {

    // Sampling in rotational direction (uniform)
    const unsigned int uSamples = samples;
    double uStep = TWOPI / (double)uSamples;
    const double halfHeight = rInner * acosh(rOuter / rInner);

    // Pre-computation for sampling in vertical direction
    const double vFactor = SQRT3_2 * uStep * rInner;
    const unsigned int vSamples = 2 * halfHeight / vFactor;
    // rescale to [-height/2, height/2]
    const double rescale = 2 * halfHeight / (vSamples * vFactor);
    const unsigned int uvSamples = uSamples * vSamples;

    // Create object
    name = "RegularCatenoid";
    reserveSpace(uvSamples, 2*uvSamples);        


    for (unsigned int v = 0; v < vSamples; ++v) {
        for (unsigned int u = 0; u < uSamples; ++u) {
            // Place vertices
            double uu = (u - (v%2)*.5)*uStep;
            double vv = (v * vFactor - halfHeight) * rescale;
            const double cat = rInner * cosh(vv / rInner);
            GLdouble x = cat * cos(uu);
            GLdouble y = cat * sin(uu);
            GLdouble z = vv;
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