#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Trackball {
    private:
        unsigned int width, height;
        const GLfloat fov = 45, near = .1, far = 10;
        const float radius = 200;   // on-screen radius in pixels
        const float r2 = glm::pow(radius, 2);
        const glm::mat4 I = glm::mat4(1.0);

        glm::vec3 pan = glm::vec3(0, 0, -5);
        glm::quat rot = glm::normalize(glm::quat(1, 0, 0, 1));
        float scaleParameter = 0;
        float scale = 1;
        glm::mat4 vMat = glm::mat4(1.0);
        glm::mat4 pMat = glm::mat4(1.0);
        glm::mat4 mvpMat = glm::mat4(1.0);

        glm::vec3 viewDir = glm::vec3(0.0, 0.0, 1.0);

        void update();

    public:
        const GLfloat* mvpPtr() const { return glm::value_ptr(mvpMat); }
        glm::vec3 viewDirection() const { return viewDir; }
        void setDimensions(int, int);
        inline void getDimensions(unsigned int& w, unsigned int& h) const {
            w = width;
            h = height;
        }
        glm::vec3 getPoint(int, int);
        void dragged(int, int, int, int);
        void panning(int, int, int, int);

        // Scale calculation
        void zoom(float);
};

#endif