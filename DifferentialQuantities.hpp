#ifndef DIFFQUANT_H
#define DIFFQUANT_H

#include <glm/glm.hpp>

class DifferentialQuantities {
    const glm::vec3 xu, xv, xuu, xuv, xvv;
    glm::mat2 ff1, ff2, sh;
    glm::vec3 nrm;      // normal
    float crvH, crvK;    // curvatures

    public:
        DifferentialQuantities(glm::vec3 xu, glm::vec3 xv,
            glm::vec3 xuu, glm::vec3 xuv, glm::vec3 xvv) :
            xu(xu), xv(xv), xuu(xuu), xuv(xuv), xvv(xvv) {
            
            // Compute IFF
            const float E = glm::dot(xu, xu);
            const float F = glm::dot(xu, xv);
            const float G = glm::dot(xv, xv);
            nrm = glm::normalize(glm::cross(xv, xu));

            // Compute IIFF
            const float L = glm::dot(xuu, nrm);
            const float M = glm::dot(xuv, nrm);
            const float N = glm::dot(xuu, nrm);

            // Principal curvatures as eigenvalues of Sh
            const float d = 1/(E*G - F*F);
            crvH = (L*G - 2*M*F + N*E) * d / 2;
            crvK = (L*N - M*M) * d;
        }

        const glm::vec3 normal() const { return nrm; }
        const float meanCurvature() const { return crvH; }
        const float gaussianCurvature() const { return crvK; }
};

#endif