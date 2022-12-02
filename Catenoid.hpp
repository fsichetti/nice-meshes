#ifndef CATENOID_H
#define CATENOID_H

#include "Mesh.hpp"
#include "Constants.hpp"

class Catenoid : public Mesh {
    public:
        Catenoid(
            unsigned int samples,   // samples in rotational direction
            double rOuter,
            double rInner
        );
};

#endif