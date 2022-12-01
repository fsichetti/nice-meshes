#include "Generators.hpp"

namespace generators {
    const double H = sqrt(3)/2;
    const double TWOPI = 2 * M_PI;


    // REGULAR TORUS //
    Mesh* regularTorus(
            unsigned int samples,   // samples in toroidal direction
            double rOuter,
            double rInner
        ) {

        const double rRatio = rOuter / rInner;

        // Sampling in toroidal direction (uniform)
        const unsigned int uSamples = samples;
        double uStep = TWOPI / (double)uSamples;

        // Pre-computation for sampling in poloidal direction
        double phi = 0;
        unsigned int count = 0;
        const double vFactor = H * uStep;
        // rescale to [0, 2pi]
        while (phi < TWOPI || count % 2) { 
            phi += vFactor * (rRatio + cos(phi));
            count++;
        }
        std::cout << count << std::endl;
        const double rescale = TWOPI / phi;
        const unsigned int vSamples = count;
        const unsigned int uvSamples = uSamples * vSamples;

        // Create object
        Mesh* m = new Mesh(true, true);
        m->name = "RegularTorus";
        m->reserveSpace(uvSamples, 2*uvSamples);        


        phi = 0;
        for (unsigned int v = 0; v < vSamples; ++v) {
            for (unsigned int u = 0; u < uSamples; ++u) {
                // Place vertices
                double uu = (u - (v%2)*.5)*uStep;
                double vv = phi * rescale;
                GLdouble x = cos(uu) * (rOuter + rInner * cos(vv));
                GLdouble y = rInner * sin(vv);
                GLdouble z = sin(uu) * (rOuter + rInner * cos(vv));
                m->addVertex(x, y, z);

                // Add faces
                unsigned int us = (u+uSamples-v%2)%uSamples;
                GLuint a = u+v*uSamples;
                GLuint b = (u+1)%uSamples+v*uSamples;
                GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
                GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

                m->addFace(a, c, b);
                m->addFace(a, d, c);
            }
            phi += vFactor * (rRatio + cos(phi));
        }

        m->finalize();
        return m;
    }


    // REGULAR CATENOID //
    Mesh* regularCatenoid(
        unsigned int samples,   // samples in rotational direction
        double rOuter,
        double rInner
    ) {
        // Sampling in rotational direction (uniform)
        const unsigned int uSamples = samples;
        double uStep = TWOPI / (double)uSamples;
        const double halfHeight = rInner * acosh(rOuter / rInner);
        std::cout << halfHeight << std::endl;

        // Pre-computation for sampling in vertical direction
        const double vFactor = H * uStep * rInner;
        const unsigned int vSamples = 2 * halfHeight / vFactor;
        // rescale to [-height/2, height/2]
        const double rescale = 2 * halfHeight / (vSamples * vFactor);
        const unsigned int uvSamples = uSamples * vSamples;

        // Create object
        Mesh* m = new Mesh(true, true);
        m->name = "RegularCatenoid";
        m->reserveSpace(uvSamples, 2*uvSamples);        


        for (unsigned int v = 0; v < vSamples; ++v) {
            for (unsigned int u = 0; u < uSamples; ++u) {
                // Place vertices
                double uu = (u - (v%2)*.5)*uStep;
                double vv = (v * vFactor - halfHeight) * rescale;
                const double cat = rInner * cosh(vv / rInner);
                GLdouble x = cat * cos(uu);
                GLdouble y = cat * sin(uu);
                GLdouble z = vv;
                m->addVertex(x, y, z);

                // Add faces
                if (v != vSamples-1) {    // Open at the ends
                    unsigned int us = (u+uSamples-v%2)%uSamples;
                    GLuint a = u+v*uSamples;
                    GLuint b = (u+1)%uSamples+v*uSamples;
                    GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
                    GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

                    m->addFace(a, b, c);
                    m->addFace(a, c, d);
                }
            }
        }

        m->finalize();
        return m;
    }
}