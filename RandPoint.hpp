#ifndef RANDPOINT_H
#define RANDPOINT_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

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

    glm::dvec2 inTriangle(glm::dvec2 v1, glm::dvec2 v2, glm::dvec2 v3);
}

#endif