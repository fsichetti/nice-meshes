#include "Torus.hpp"

Torus::Torus(
        unsigned int samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true) {

    // Construct torus
    const double rRatio = rOuter / rInner;
    const unsigned int uSamples = samples;
    double uStep = TWOPI / (double)uSamples;

    // Pre-computation for sampling in poloidal direction
    double phi = 0;
    unsigned int count = 0;
    const double vFactor = SQRT3_2 * uStep;
    // rescale to [0, 2pi]
    while (phi < TWOPI || count % 2) { 
        phi += vFactor * (rRatio + cos(phi));
        ++count;
    }
    const double rescale = TWOPI / phi;
    const unsigned int vSamples = count;
    const unsigned int uvSamples = uSamples * vSamples;

    name = "RegularTorus";
    reserveSpace(uvSamples, 2*uvSamples);        


    phi = 0;
    for (unsigned int v = 0; v < vSamples; ++v) {
        for (unsigned int u = 0; u < uSamples; ++u) {
            // Place vertices
            double uu = (u - (v%2)*.5)*uStep;
            double vv = phi * rescale;
            GLdouble x = cos(uu) * (rOuter + rInner * cos(vv));
            GLdouble y = rInner * sin(vv);
            GLdouble z = sin(uu) * (rOuter + rInner * cos(vv));
            addVertex(x, y, z);

            // Add faces
            unsigned int us = (u+uSamples-v%2)%uSamples;
            GLuint a = u+v*uSamples;
            GLuint b = (u+1)%uSamples+v*uSamples;
            GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
            GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

            addFace(a, c, b);
            addFace(a, d, c);
        }
        phi += vFactor * (rRatio + cos(phi));
    }

    finalize();
}