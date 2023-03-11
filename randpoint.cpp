#include "RandPoint.hpp"

glm::dvec3 RandPoint::inTriangle(glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 v3) {
	const double p = glm::linearRand<double>(0, 1);
	const double q = glm::linearRand<double>(0, 1);
	const double dif = std::abs(p-q);
	const double a = (p + q - dif) / 2;
	const double b = dif;
	const double c = 1 - (p + q + dif) / 2;
	return glm::dvec1(a) * v1 + glm::dvec1(b) * v2 + glm::dvec1(c) * v3;
}  