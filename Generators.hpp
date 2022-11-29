#ifndef GENERATORS_H
#define GENERATORS_H

#include "Mesh.hpp"
#include <math.h>

namespace generators {
    Mesh* regularTorus(unsigned int, double, double);
    Mesh* regularCatenoid(unsigned int, double, double);
}

#endif