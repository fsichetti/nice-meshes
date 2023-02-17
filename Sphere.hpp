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

        double laplacian(double u, double v, double f, double fu,
            double fv, double fuu, double fuv, double fvv) const override;
            
    private:
        void initIcosahedron();
        void computeValues();
        const double radius;
};

#endif