#ifndef DIFFQUANT_H
#define DIFFQUANT_H

#include <glm/glm.hpp>

class DifferentialQuantities {
    glm::vec3 nrm;      // normal
    float crvH, crvK;    // curvatures

    public:
        DifferentialQuantities(glm::vec3 xu, glm::vec3 xv,
            glm::vec3 xuu, glm::vec3 xuv, glm::vec3 xvv) {
                
            // Compute IFF
            const float E = glm::dot(xu, xu);
            const float F = glm::dot(xu, xv);
            const float G = glm::dot(xv, xv);
            nrm = glm::normalize(glm::cross(xu, xv));

            // Compute IIFF
            const float L = glm::dot(xuu, nrm);
            const float M = glm::dot(xuv, nrm);
            const float N = glm::dot(xvv, nrm);

            // Compute curvatures
            const float det = (E*G - F*F);
            const float idet = 1/det;
            crvH = (G*L - 2*F*M + E*N) * idet / 2;
            crvK = (L*N - M*M) * idet;
        }

        const glm::vec3 normal() const { return nrm; }
        const float meanCurvature() const { return crvH; }
        const float gaussianCurvature() const { return crvK; }
};

#endif