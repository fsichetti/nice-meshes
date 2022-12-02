#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "Mesh.hpp"
#include "Constants.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

class BezierPatch : public Mesh {
    public:
        static const unsigned int degree = 3;  // hardcoded as cubic patches

        // Control grid struct
        class ControlGrid;

        BezierPatch(
            ControlGrid cg,
            unsigned int samples    // samples per uv direction
        );

    private:
        // Bernstein polynomials
        unsigned int binomial[degree+1];
        void cacheBinomials();
        inline double bPoly(unsigned int i, double x) const {
            return binomial[i] * pow(x, i) * pow(1-x, degree-i);
        }
};



class BezierPatch::ControlGrid {
    public:
        ControlGrid() {};   // empty constructor
        ControlGrid(double maxNorm);   // random generator

        inline glm::vec3 get(unsigned int i, unsigned int j) const {
            const auto id = i + (degree+1) * j;
            return glm::vec3(storage[0][id], storage[1][id], storage[2][id]);
        }
        inline void set(unsigned int i, unsigned int j, const glm::vec3& val) {
            const auto id = i + (degree+1) * j;
            storage[0][id] = val[0];
            storage[1][id] = val[1];
            storage[2][id] = val[2];
        }
        inline double& at(unsigned int i, unsigned int j,
            unsigned int coordinate) {
            return storage[coordinate][i + (degree+1)*j];
        }

    private:
        double storage[3][(degree+1)*(degree+1)];
        // Random number generation
        glm::vec3 pointInSphere(const glm::vec3& center, double radius) {
            glm::vec3 v(0);
            v[0] = glm::gaussRand(0., radius);
            v[1] = glm::gaussRand(0., radius);
            v[2] = glm::gaussRand(0., radius);
            
            return v + center;
        }
        glm::vec3 pointOnSphere(const glm::vec3& center, double radius) {
            glm::vec3 v = glm::sphericalRand(radius);
            return v + center;
        }
};

#endif