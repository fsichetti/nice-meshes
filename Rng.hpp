#ifndef RNG_H
#define RNG_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

    // Random number generation
namespace random3d {
    inline glm::vec3 gaussian(const glm::vec3& center, double variance) {
        return glm::vec3(
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance),
            glm::gaussRand<float>(0., variance)
        ) + center;
    }
    
    inline glm::vec3 pointOnSphere(const glm::vec3& center, double radius) {
        return glm::sphericalRand<float>(radius) + center;
    }

    inline glm::vec3 pointInSphere(const glm::vec3& center, double radius) {
        return pointOnSphere(center, radius) *
            static_cast<float>(pow(glm::linearRand<double>(0,1), 1./3.));
    }
}

#endif