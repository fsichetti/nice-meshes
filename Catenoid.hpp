#ifndef CATENOID_H
#define CATENOID_H

#include "Mesh.hpp"
#include <math.h>

class Catenoid : public Mesh {
    public:
        Catenoid(
            unsigned int samples,   // samples in rotational direction
            double rOuter,
            double rInner
        );
    private:
        const double H = sqrt(3)/2;
        const double TWOPI = 2 * M_PI;
};

#endif