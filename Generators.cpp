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
        const double factor = H * uStep;
        // rescale to [0, 2pi]
        while (phi < TWOPI || count % 2) { 
            phi += factor * (rRatio + cos(phi));
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
            phi += factor * (rRatio + cos(phi));
        }

        m->finalize();
        return m;
    }


    // REGULAR CATENOID //
    Mesh* regularCatenoid(
        unsigned int samples,   // samples in rotational direction
        double radius,
        double height
    ) {
        // Sampling in rotational direction (uniform)
        const unsigned int uSamples = samples;
        double uStep = TWOPI / (double)uSamples;
        const double& c = radius;

        // Pre-computation for sampling in vertical direction
        double z = -height/2;
        unsigned int count = 0;
        // rescale to [0, 2pi]
        while (z < height/2) { 
            std::cout << z << std::endl;
            z += H * uStep * cosh(z/c);
            count++;
        }
        std::cout << z << std::endl;
        const double scaleFac = TWOPI * H * uStep / z;
        const unsigned int vSamples = count;
        const unsigned int uvSamples = uSamples * vSamples;

        // Create object
        Mesh* m = new Mesh(true, true);
        m->name = "RegularCatenoid";
        m->reserveSpace(uvSamples, 2*uvSamples);        


        z = -height/2;
        for (unsigned int v = 0; v < vSamples; ++v) {
            for (unsigned int u = 0; u < uSamples; ++u) {
                // Place vertices
                double uu = (u - (v%2)*.5)*uStep;
                double vv = z;
                const double k = cosh(vv / c);
                GLdouble x = c * k * cos(uu);
                GLdouble y = c * k * sin(uu);
                GLdouble z = vv;
                m->addVertex(x, y, z);
                // std::cout << x << ", " << y << ", " << z << std::endl;

                // Add faces
                if (v != vSamples) {    // Open at the ends
                    unsigned int us = (u+uSamples-v%2)%uSamples;
                    GLuint a = u+v*uSamples;
                    GLuint b = (u+1)%uSamples+v*uSamples;
                    GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
                    GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

                    m->addFace(a, b, c);
                    m->addFace(a, c, d);
                }
            }
            z += scaleFac * cosh(z/c);
        }

        m->finalize();
        return m;
    }
}