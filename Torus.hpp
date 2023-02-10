#ifndef TORUS_H
#define TORUS_H

#include "Mesh.hpp"
#include "PlaneSampling.hpp"
#include "Constants.hpp"
#include "DifferentialQuantities.hpp"

class Torus : public Mesh {
    public:
        Torus(
            unsigned int samples,   // samples in toroidal direction
            double rOuter,
            double rInner
        );
        Torus(
            std::string path,
            double rOuter,
            double rInner
        );

    private:
        const double rInner, rOuter;
        unsigned int placeVertex(double u, double v);
};

#endif