#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "Mesh.hpp"
#include "Constants.hpp"
#include "RandPoint.hpp"
#include "PlaneSampling.hpp"
#include <glm/glm.hpp>

class BezierPatch : public Mesh {
    public:
        static const uint degree = 3;  // hardcoded as cubic patches

        // Control grid and plane sampling classes
        class ControlGrid;

        BezierPatch(
            const ControlGrid *const cg,
            uint samples    // samples per uv direction
        );

        BezierPatch(
            const ControlGrid *const cg,
            PlaneSampling smp    // provided sampling of the plane
        );

        BezierPatch(
            const ControlGrid *const cg,
            uint samples,
            double anisotropy
        );

        ~BezierPatch();

        DifferentialQuantities diffEvaluate(double u, double v) const override;
            

    private:
        const ControlGrid *const control;
        // Binomial coefficients
        uint *bc;
        uint binomial(int k, int n) const;
        void bcPrepare();

        // Bernstein polynomials with derivatives
        inline double bPoly(int i, int n, double x,
            uint derivative = 0) const {
            if (derivative == 0) {
                if (i < 0 || i > n) return 0;
                return binomial(i, n) * pow(x, i) * pow(1-x, n-i);
            }
            return n * (
                bPoly(i-1, n-1, x, derivative-1)
                - bPoly(i, n-1, x, derivative-1)
            );
        }

        // Computations
        glm::dvec3 sampleSurface(double u, double v,
            uint derivU = 0, uint derivV = 0) const;
};



class BezierPatch::ControlGrid {
    public:
        ControlGrid() {};   // empty constructor
        ControlGrid(        // random generator
            double baseScale,
            double borderBumpiness=1.0, // may self-intersect for values >1
            double innerBumpiness=-1.0  // if <0, it's equal to borderBump.
        );

        inline glm::dvec3 get(uint i, uint j) const {
            const auto id = i + (degree+1) * j;
            return glm::dvec3(storage[0][id], storage[1][id], storage[2][id]);
        }
        inline void set(uint i, uint j, const glm::dvec3& val) {
            const auto id = i + (degree+1) * j;
            storage[0][id] = val[0];
            storage[1][id] = val[1];
            storage[2][id] = val[2];
        }
        inline double& at(uint i, uint j,
            uint coordinate) {
            return storage[coordinate][i + (degree+1)*j];
        }

        void writeCoordinate(std::string pathx, int coordinate);

    private:
        double storage[3][(degree+1)*(degree+1)];
};

#endif