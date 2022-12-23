#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "Mesh.hpp"
#include "Constants.hpp"
#include "RandPoint.hpp"
#include <glm/glm.hpp>

class BezierPatch : public Mesh {
    public:
        static const unsigned int degree = 3;  // hardcoded as cubic patches

        // Control grid and plane sampling classes
        class ControlGrid;
        class PlaneSampling;

        BezierPatch(
            ControlGrid cg,
            unsigned int samples    // samples per uv direction
        );

        BezierPatch(
            ControlGrid cg,
            PlaneSampling smp    // provided sampling of the plane
        );

    private:
        // Bernstein polynomials
        unsigned int bc[((degree-1) * (degree-1) + degree-1) / 2] = {};
        unsigned int binomial(int k, int n) {
            if (k < 0 || k > n) return 0;
            if (k == 0 || k == n) return 1;
            const unsigned int nn = n-2;
            const unsigned int ind = (nn*nn + nn)/2 + k-1;
            if (bc[ind] == 0) bc[ind] = binomial(k-1, n-1) + binomial(k, n-1);
            return bc[ind];
        }
        inline double bPoly(unsigned int i, double x) {
            return binomial(i, degree) * pow(x, i) * pow(1-x, degree-i);
        }
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

struct BezierPatch::PlaneSampling {
    Mesh::vArray verts;     // vertices in 2D
    Mesh::fArray faces;
    void readFromObj(std::string path);
};

#endif