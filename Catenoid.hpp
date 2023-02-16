#ifndef CATENOID_H
#define CATENOID_H

#include "Mesh.hpp"
#include "Constants.hpp"
#include "PlaneSampling.hpp"
#include "DifferentialQuantities.hpp"

class Catenoid : public Mesh {
    public:
        Catenoid(
            unsigned int samples,   // samples in rotational direction
            double rOuter,
            double rInner
        );
        Catenoid(
            std::string path,
            double rOuter,
            double rInner
        );
        double laplacian(double u, double v, double f, double fu,
            double fv, double fuu, double fuv, double fvv) const override;

    private:
        const double rInner, rOuter, height;
        unsigned int placeVertex(double u, double v);
};

#endif