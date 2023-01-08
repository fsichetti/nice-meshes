#include "Torus.hpp"

Torus::Torus(
        unsigned int samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true, true) {

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
            const double uu = (u - (v%2)*.5) * uStep;
            const double vv = phi / phiMax;
            const double sinu = sin(TWOPI * uu);
            const double cosu = cos(TWOPI * uu);
            const double sinv = sin(TWOPI * vv);
            const double cosv = cos(TWOPI * vv);
            
            const unsigned int index = addVertex();
            
            attrib(index, Attribute::X) = cosu * (rOuter + rInner * cosv);
            attrib(index, Attribute::Y) = sinu * (rOuter + rInner * cosv);
            attrib(index, Attribute::Z) = rInner * sinv;

            // Normals
            attrib(index, Attribute::NX) = cosu * cosv;
            attrib(index, Attribute::NY) = sinu * cosv;
            attrib(index, Attribute::NZ) = sinv;

            // Parametric coordinates
            attrib(index, Attribute::U) = uu;
            attrib(index, Attribute::V) = vv;

            // Curvature
            attrib(index, Attribute::K) = cosv /
                (rInner * (rOuter + rInner * cosv));
            attrib(index, Attribute::H) = (rOuter + 2 * rInner * cosv) /
                (2 * rInner * (rOuter + rInner * cosv));

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
    computeNormals(true);
}