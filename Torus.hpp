#ifndef TORUS_H
#define TORUS_H

#include "Mesh.hpp"
#include "Constants.hpp"
#include "DifferentialQuantities.hpp"

class Torus : public Mesh {
    public:
        Torus(
            unsigned int samples,   // samples in toroidal direction
            double rOuter,
            double rInner
        );
};

#endif