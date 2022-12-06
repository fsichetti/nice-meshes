#ifndef RANDPOINT_H
#define RANDPOINT_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

// Random number generation
namespace RandPoint {
    inline glm::vec3 gaussian3(double variance,
        const glm::vec3& center = glm::vec3()) {
        return glm::vec3(
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance)
        ) + center;
    }
    
    inline glm::vec3 onSphere(double radius,
        const glm::vec3& center = glm::vec3()) {
        return glm::sphericalRand<float>(radius) + center;
    }

    inline glm::vec3 inSphere(double radius,
        const glm::vec3& center = glm::vec3()) {
        return glm::ballRand<float>(radius) + center;
    }
}

#endif