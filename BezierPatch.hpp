#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "Mesh.hpp"
#include <math.h>

class BezierPatch : public Mesh {
    public:
        // Control grid struct
        struct ControlGrid {
            double pt[3][16];
            inline double& x(unsigned int id) { return pt[0][id]; }
            inline double& y(unsigned int id) { return pt[1][id]; }
            inline double& z(unsigned int id) { return pt[2][id]; }
        };

        BezierPatch(
            ControlGrid cg,
            unsigned int samples    // samples per uv direction
        );
};

#endif