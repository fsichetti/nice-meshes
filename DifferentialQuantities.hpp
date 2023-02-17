#ifndef DIFFQUANT_H
#define DIFFQUANT_H

#include <glm/glm.hpp>

class DifferentialQuantities {
    private:
        glm::vec3 nrm;      // normal
        double crvH, crvK;    // curvatures
        double E, F, G, L, M, N;
        double detg;

    public:
        DifferentialQuantities(glm::vec3 xu, glm::vec3 xv,
            glm::vec3 xuu, glm::vec3 xuv, glm::vec3 xvv) {
                
            // Compute IFF
            E = glm::dot(xu, xu);
            F = glm::dot(xu, xv);
            G = glm::dot(xv, xv);

            // Compute IIFF
            nrm = glm::normalize(glm::cross(xu, xv));
            L = glm::dot(xuu, nrm);
            M = glm::dot(xuv, nrm);
            N = glm::dot(xvv, nrm);

            // Compute curvatures
            detg = (E*G - F*F);
            const double idet = 1/detg;
            crvH = (G*L - 2*F*M + E*N) * idet / 2;
            crvK = (L*N - M*M) * idet;
        }

        const glm::vec3 normal() const { return nrm; }
        const double meanCurvature() const { return crvH; }
        const double gaussianCurvature() const { return crvK; }
        const glm::mat2 metric() const { return glm::mat2(E, F, F, G); }
};

#endif