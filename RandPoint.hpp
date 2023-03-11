#ifndef RANDPOINT_H
#define RANDPOINT_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <random>

// Random number generation
namespace RandPoint {
    inline uint seed(uint seed = time(0)) {
        srand(seed); // get random seed
        return seed;
    }

    inline glm::dvec1 gaussian1(double variance) {
        return glm::dvec1(
            glm::gaussRand<double>(0., variance)
        );
    }

    inline glm::dvec2 gaussian2(double variance) {
        return glm::dvec2(
            glm::gaussRand<double>(0., variance),
            glm::gaussRand<double>(0., variance)
        );
    }

    inline glm::dvec3 gaussian3(double variance) {
        return glm::dvec3(
            glm::gaussRand<double>(0., variance),
            glm::gaussRand<double>(0., variance),
            glm::gaussRand<double>(0., variance)
        );
    }
    
    inline glm::dvec3 onSphere(double radius) {
        return glm::sphericalRand<double>(radius);
    }

    inline glm::dvec3 inSphere(double radius) {
        return glm::ballRand<double>(radius);
    }

    inline glm::dvec3 inTriangle(glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 v3) {
        const double p = glm::linearRand<double>(0, 1);
        const double q = glm::linearRand<double>(0, 1);
        const double dif = std::abs(p-q);
        const double a = (p + q - dif) / 2;
        const double b = dif;
        const double c = 1 - (p + q + dif) / 2;
        return glm::dvec1(a) * v1 + glm::dvec1(b) * v2 + glm::dvec1(c) * v3;
    }
}

#endif