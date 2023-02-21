#include "DifferentialQuantities.hpp"

DifferentialQuantities::DifferentialQuantities(glm::vec3 xu, glm::vec3 xv,
	glm::vec3 xuu, glm::vec3 xuv, glm::vec3 xvv) :
	xu(xu), xv(xv), xuu(xuu), xuv(xuv), xvv(xvv) {
		
	// Compute IFF
	E = glm::dot(xu, xu);
	F = glm::dot(xu, xv);
	G = glm::dot(xv, xv);

	g = glm::mat2(E, F, F, G);
	invg = inverse(g);
	detg = determinant(g);

	// Compute IIFF
	nrm = glm::normalize(glm::cross(xu, xv));
	L = glm::dot(xuu, nrm);
	M = glm::dot(xuv, nrm);
	N = glm::dot(xvv, nrm);
}

glm::vec3 DifferentialQuantities::gradient(double f, double fu, double fv)
	const {
    const glm::vec2 df(fu, fv);
    const glm::vec2 components = inverse(g) * df;
    const glm::vec3 grad = components[0] * xu + components[1] * xv;

    return grad;
}



double DifferentialQuantities::laplacian(double f, double fu, double fv,
	double fuu, double fuv, double fvv) const {
    const double detg = E*G - F*F;
    const double g_deltau = -(E * (G * dot(xu, xvv) - F * dot(xv, xvv)) +
            2 * F * (F * dot(xv, xuv) - G * dot(xu, xuv)) +
            G * (G * dot(xu, xuu) - F * dot(xv, xuu))) /
            pow(detg, 2);
        
    const double g_deltav = -(E * (E * dot(xv, xvv) - F * dot(xu, xvv)) +
            2 * F * (F * dot(xu, xuv) - E * dot(xv, xuv)) +
            G * (E * dot(xv, xuu) - F * dot(xu, xuu))) /
            pow(detg, 2);
    const double g_deltauu = G / detg;
    const double g_deltauv = -2 * F / detg;
    const double g_deltavv = E / detg;

    const double lap = g_deltau * fu + g_deltav * fv + g_deltauu * fuu + g_deltauv * fuv +
        g_deltavv * fvv;
    return std::isnan(lap) ? 0 : lap;
}


	
glm::vec3 DifferentialQuantities::hessian(double f, double fu, double fv,
	double fuu, double fuv, double fvv) const {
    glm::vec2 I(fu, fv);
    glm::mat2 II(fuu, fuv, fuv, fvv);
    glm::mat2 G[2];
    G[0] = glm::mat2(dot(xu, xuu), dot(xv, xuu), dot(xu, xuv), dot(xv, xuv));
    G[1] = glm::mat2(dot(xu, xuv), dot(xv, xuv), dot(xu, xvv), dot(xv, xvv));

    glm::mat2 H(0);
    for (auto i = 0; i < 2; ++i) {
        for (auto k = 0; k < 2; ++k) {
            // mat[k] accesses k-th column (col-maj)
            H[i][k] = dot(transpose(invg)[k], (II[i] - G[i] * invg * I));
        }
    }
    const glm::vec2 J2 = H * invg * I;
    const glm::vec3 J = J2[0] * xu + J2[1] * xv;
    return J;
}