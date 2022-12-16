#include "Torus.hpp"

Torus::Torus(
        unsigned int samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true) {

    // Construct torus
    const double rRatio = rOuter / rInner;
    const unsigned int uSamples = samples;
    const double uStep = 1.0 / static_cast<double>(uSamples);

    // Pre-computation for sampling in poloidal direction
    double phiMax = 0;
    unsigned int count = 0;
    const double vStep = SQRT3_2 * uStep;
    // rescale to [0, 1]
    while (phiMax < TWOPI || count % 2) { 
        phiMax += TWOPI * vStep * (rRatio + cos(phiMax));
        ++count;
    }
    const unsigned int vSamples = count;
    const unsigned int uvSamples = uSamples * vSamples;

    name = "RegularTorus";
    reserveSpace(uvSamples, 2*uvSamples);    


    double phi = 0;
    for (unsigned int v = 0; v < vSamples; ++v) {
        for (unsigned int u = 0; u < uSamples; ++u) {
            // Place vertices
            double uu = (u - (v%2)*.5) * uStep;
            double vv = phi / phiMax;
            double upi = TWOPI * uu, vpi = TWOPI * vv;
            GLdouble x = cos(upi) * (rOuter + rInner * cos(vpi));
            GLdouble y = rInner * sin(vpi);
            GLdouble z = sin(upi) * (rOuter + rInner * cos(vpi));
            addVertex(x, y, z);

            // Write parametric coordinates
            const unsigned int index = u + v*uSamples;
            attrib(index, Attribute::U) = uu;
            attrib(index, Attribute::V) = vv;

            // Add faces
            unsigned int us = (u+uSamples-v%2)%uSamples;
            GLuint a = index;
            GLuint b = (u+1)%uSamples+v*uSamples;
            GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
            GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

            addFace(a, c, b);
            addFace(a, d, c);
        }
        phi += TWOPI * vStep * (rRatio + cos(phi));
    }
}