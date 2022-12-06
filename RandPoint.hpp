#ifndef RANDPOINT_H
#define RANDPOINT_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

// Random number generation
namespace RandPoint {
    inline glm::vec3 gaussian3(const glm::vec3& center, double variance) {
        return glm::vec3(
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance)
        ) + center;
    }
    
    inline glm::vec3 onSphere(const glm::vec3& center, double radius) {
        return glm::sphericalRand<float>(radius) + center;
    }

    inline glm::vec3 inSphere(const glm::vec3& center, double radius) {
        return onSphere(center, radius) *
            static_cast<float>(pow(glm::linearRand<double>(0,1), 1./3.));
    }

    inline glm::vec2 inDisk() {
        return glm::diskRand(1);
    }
}

#endif