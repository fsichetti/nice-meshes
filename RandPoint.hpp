#ifndef RANDPOINT_H
#define RANDPOINT_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

// Random number generation
namespace RandPoint {
    inline unsigned int seed(unsigned int seed = time(0)) {
        srand(seed); // get random seed
        return seed;
    }

    inline glm::vec1 gaussian1(double variance) {
        return glm::vec1(
            glm::gaussRand<float>(0., variance)
        );
    }

    inline glm::vec2 gaussian2(double variance) {
        return glm::vec2(
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance)
        );
    }

    inline glm::vec3 gaussian3(double variance) {
        return glm::vec3(
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance)
        );
    }
    
    inline glm::vec3 onSphere(double radius) {
        return glm::sphericalRand<float>(radius);
    }

    inline glm::vec3 inSphere(double radius) {
        return glm::ballRand<float>(radius);
    }
}

#endif