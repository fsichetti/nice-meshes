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

    inline glm::vec1 gaussian1(double variance) {
        return glm::vec1(
            glm::gaussRand<double>(0., variance)
        );
    }

    inline glm::vec2 gaussian2(double variance) {
        return glm::vec2(
            glm::gaussRand<double>(0., variance),
            glm::gaussRand<double>(0., variance)
        );
    }

    inline glm::vec3 gaussian3(double variance) {
        return glm::vec3(
            glm::gaussRand<double>(0., variance),
            glm::gaussRand<double>(0., variance),
            glm::gaussRand<double>(0., variance)
        );
    }
    
    inline glm::vec3 onSphere(double radius) {
        return glm::sphericalRand<double>(radius);
    }

    inline glm::vec3 inSphere(double radius) {
        return glm::ballRand<double>(radius);
    }

    inline glm::vec3 inTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
        const double p = glm::linearRand<double>(0, 1);
        const double q = glm::linearRand<double>(0, 1);
        const double dif = std::abs(p-q);
        const double a = (p + q - dif) / 2;
        const double b = dif;
        const double c = 1 - (p + q + dif) / 2;
        return glm::vec1(a) * v1 + glm::vec1(b) * v2 + glm::vec1(c) * v3;
    }
}

#endif