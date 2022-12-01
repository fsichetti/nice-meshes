#ifndef TORUS_H
#define TORUS_H

#include "Mesh.hpp"
#include <math.h>

class Torus : public Mesh {
    public:
        Torus(
            unsigned int samples,   // samples in toroidal direction
            double rOuter,
            double rInner
        );
    private:
        const double H = sqrt(3)/2;
        const double TWOPI = 2 * M_PI;
};

#endif