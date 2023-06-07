#ifndef CATENOID_H
#define CATENOID_H

#include "Mesh.hpp"
#include "Constants.hpp"
#include "PlaneSampling.hpp"
#include "DifferentialQuantities.hpp"

class Catenoid : public Mesh {
    public:
        Catenoid(
            uint samples,   // samples in rotational direction
            double rOuter,
            double rInner,
            bool quad = false
        );
        Catenoid(
            const PlaneSampling& plane,
            double rOuter,
            double rInner
        );
        Catenoid(
            std::string path,
            double rOuter,
            double rInner
        );
        Catenoid(
            uint samples,
            double rOuter,
            double rInner,
            double anisotropy
        );
        DifferentialQuantities diffEvaluate(double u, double v) const override;

    private:
        const double rInner, rOuter, height;
        uint placeVertex(double u, double v);
        void replaceVertex(uint index, double u, double v);
};

#endif