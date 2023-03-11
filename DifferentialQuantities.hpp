#ifndef DIFFQUANT_H
#define DIFFQUANT_H

#include <glm/glm.hpp>

class DifferentialQuantities {
    private:
        const glm::dvec3 xu, xv, xuu, xuv, xvv;
        glm::dvec3 nrm;      // normal
        double E, F, G, L, M, N;
        glm::mat2 g, invg;
        double detg;

    public:
        DifferentialQuantities(glm::dvec3 xu, glm::dvec3 xv,
            glm::dvec3 xuu, glm::dvec3 xuv, glm::dvec3 xvv);

        glm::dvec3 normal() const { return nrm; }
        double meanCurvature() const { return (G*L - 2*F*M + E*N) / (detg * 2); }
        double gaussianCurvature() const { return (L*N - M*M) / detg; }

        glm::dvec3 gradient(double f, double fu, double fv) const;
        double laplacian(double f, double fu, double fv,
            double fuu, double fuv, double fvv) const;
        glm::dvec3 hessian(double f, double fu, double fv,
            double fuu, double fuv, double fvv) const;
};

#endif