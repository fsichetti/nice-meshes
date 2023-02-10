#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "Mesh.hpp"
#include "Constants.hpp"
#include "RandPoint.hpp"
#include "DifferentialQuantities.hpp"
#include "PlaneSampling.hpp"
#include <glm/glm.hpp>

class BezierPatch : public Mesh {
    public:
        static const unsigned int degree = 3;  // hardcoded as cubic patches

        // Control grid and plane sampling classes
        class ControlGrid;

        BezierPatch(
            ControlGrid cg,
            unsigned int samples    // samples per uv direction
        );

        BezierPatch(
            ControlGrid cg,
            PlaneSampling smp    // provided sampling of the plane
        );

    private:
        // Binomial coefficients
        unsigned int bc[((degree-1) * (degree-1) + degree-1) / 2] = {};
        unsigned int binomial(int k, int n);

        // Bernstein polynomials with derivatives
        inline double bPoly(int i, int n, double x,
            unsigned int derivative = 1) {
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
        glm::vec3 sampleSurface(const ControlGrid&, double u, double v,
            unsigned int derivU = 0, unsigned int derivV = 0);
};



class BezierPatch::ControlGrid {
    public:
        ControlGrid() {};   // empty constructor
        ControlGrid(        // random generator
            double baseScale,
            double borderBumpiness=1.0, // may self-intersect for values >1
            double innerBumpiness=-1.0  // if <0, it's equal to borderBump.
        );

        inline glm::vec3 get(unsigned int i, unsigned int j) const {
            const auto id = i + (degree+1) * j;
            return glm::vec3(storage[0][id], storage[1][id], storage[2][id]);
        }
        inline void set(unsigned int i, unsigned int j, const glm::vec3& val) {
            const auto id = i + (degree+1) * j;
            storage[0][id] = val[0];
            storage[1][id] = val[1];
            storage[2][id] = val[2];
        }
        inline double& at(unsigned int i, unsigned int j,
            unsigned int coordinate) {
            return storage[coordinate][i + (degree+1)*j];
        }

    private:
        double storage[3][(degree+1)*(degree+1)];
};

#endif