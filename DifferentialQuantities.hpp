#ifndef DIFFQUANT_H
#define DIFFQUANT_H

#include <glm/glm.hpp>

class DifferentialQuantities {
    private:
        const glm::vec3 xu, xv, xuu, xuv, xvv;
        glm::vec3 nrm;      // normal
        double E, F, G, L, M, N;
        glm::mat2 g, invg;
        double detg;

    public:
        DifferentialQuantities(glm::vec3 xu, glm::vec3 xv,
            glm::vec3 xuu, glm::vec3 xuv, glm::vec3 xvv);

        glm::vec3 normal() const { return nrm; }
        double meanCurvature() const { return (G*L - 2*F*M + E*N) / (detg * 2); }
        double gaussianCurvature() const { return (L*N - M*M) / detg; }

        glm::vec3 gradient(double f, double fu, double fv) const;
        double laplacian(double f, double fu, double fv,
            double fuu, double fuv, double fvv) const;
        glm::vec3 hessian(double f, double fu, double fv,
            double fuu, double fuv, double fvv) const;
};

#endif