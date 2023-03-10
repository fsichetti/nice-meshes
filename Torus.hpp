#ifndef TORUS_H
#define TORUS_H

#include "Mesh.hpp"
#include "PlaneSampling.hpp"
#include "Constants.hpp"
#include "DifferentialQuantities.hpp"

class Torus : public Mesh {
    public:
        Torus(
            uint samples,   // samples in toroidal direction
            double rOuter,
            double rInner
        );
        Torus(
            std::string path,
            double rOuter,
            double rInner
        );

        DifferentialQuantities diffEvaluate(double u, double v) const override;

    private:
        const double rInner, rOuter;
        uint placeVertex(double u, double v);
};

#endif