#ifndef SPHERE_H
#define SPHERE_H

#include "Mesh.hpp"
#include "Constants.hpp"

class Sphere : public Mesh {
    public:
        Sphere(
            unsigned int subdivisionLevel,
            double radius
        );
        Sphere(std::string path, double radius);
        
        DifferentialQuantities diffEvaluate(double u, double v) const override;

    private:
        void initIcosahedron();
        void computeValues();
        const double radius;
};

#endif