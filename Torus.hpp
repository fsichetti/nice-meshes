#ifndef TORUS_H
#define TORUS_H

#include "Mesh.hpp"
#include "Constants.hpp"

class Torus : public Mesh {
    public:
        Torus(
            unsigned int samples,   // samples in toroidal direction
            double rOuter,
            double rInner
        );
};

#endif