#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "Mesh.hpp"
#include "Constants.hpp"

class BezierPatch : public Mesh {
    public:
        static const unsigned int degree = 3;  // hardcoded as cubic patches
        // Control grid struct
        struct ControlGrid {
            double pt[3][(degree+1)*(degree+1)];
            inline double& at(unsigned int i, unsigned int j,
                unsigned int coordinate) {
                return pt[coordinate][i + (degree+1)*j];
            }
        };

        BezierPatch(
            ControlGrid cg,
            unsigned int samples    // samples per uv direction
        );

    private:
        unsigned int binomial[degree+1];
        void cacheBinomials();

        inline double bPoly(unsigned int i, double x) const {
            return binomial[i] * pow(x, i) * pow(1-x, degree-i);
        }
};

#endif