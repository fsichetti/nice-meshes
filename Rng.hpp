#ifndef RNG_H
#define RNG_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

namespace random3d {
    // Random number generation
    inline glm::vec3 pointInSphere(const glm::vec3& center, double variance) {
        return glm::vec3(
            glm::gaussRand(0., variance),
            glm::gaussRand(0., variance),
            glm::gaussRand(0., variance)
        ) + center;
    }
    inline glm::vec3 pointOnSphere(const glm::vec3& center, double radius) {
        return (glm::vec3)glm::sphericalRand(radius) + center;
    }
}

#endif