#include <glm/glm.hpp>

class DifferentialQuantities {
    const glm::vec3 xu, xv, xuu, xuv, xvv;
    glm::mat2 ff1, ff2, sh;
    glm::vec3 nrm;      // normal
    float crvH, crvK, crv1, crv2;    // curvatures

    public:
        DifferentialQuantities(glm::vec3 xu, glm::vec3 xv,
            glm::vec3 xuu, glm::vec3 xuv, glm::vec3 xvv) :
            xu(xu), xv(xv), xuu(xuu), xuv(xuv), xvv(xvv) {
            
            // Compute IFF
            ff1 = glm::mat2(
                glm::dot(xu, xu), glm::dot(xv, xu),
                glm::dot(xu, xv), glm::dot(xv, xv)
            );
            const float &E = ff1[0][0], &F = ff1[0][1], &G = ff1[1][1];
            nrm = glm::normalize(glm::cross(xv, xu));

            // Compute IIFF
            ff2 = glm::mat2(
                glm::dot(xuu, nrm), glm::dot(xuv, nrm),
                glm::dot(xuv, nrm), glm::dot(xvv, nrm)
            );
            const float &L = ff2[0][0], &M = ff2[0][1], &N = ff2[1][1];

            // Shape operator
            sh = glm::mat2(
                L*G-M*F, M*G-N*F,
                M*E-L*F, N*E-M*F
            ) / glm::determinant(ff1);

            // Principal curvatures as eigenvalues of Sh
            const float m = (sh[0][0] + sh[1][1]) / 2;
            const float tmp = sqrt(m*m - glm::determinant(sh));
            crv1 = m + tmp;
            crv2 = m - tmp;
            crvH = (crv1 + crv2) / 2;
            crvK = crv1 * crv2;
        }

        const glm::vec3 normal() const { return nrm; }
        const float meanCurvature() const { return crvH; }
        const float gaussianCurvature() const { return crvK; }
};