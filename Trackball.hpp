#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Trackball {
    private:
        uint width, height;
        const GLfloat fov = 45, near = .1, far = 10;
        const float radius = 200;   // on-screen radius in pixels
        const float r2 = glm::pow(radius, 2);
        const glm::mat4 I = glm::mat4(1.0);

        glm::vec3 pan;
        glm::quat rot;
        float scaleParameter, scale;

        glm::vec3 viewDir = glm::vec3(0.0, 0.0, 1.0);

        const glm::mat4 vMat = glm::translate(I, glm::vec3(0,0,-5));;
        glm::mat4 pMat = I;
        inline glm::mat4 tMat() const { return glm::translate(I, pan); }
        inline glm::mat4 tiMat() const { return glm::translate(I, -pan); }
        inline glm::mat4 rMat() const { return glm::toMat4(rot); }
        inline glm::mat4 riMat() const { return glm::toMat4(glm::inverse(rot)); }
        inline glm::mat4 sMat() const { return glm::scale(I, glm::vec3(scale)); }
        inline glm::mat4 siMat() const { return glm::scale(I, glm::vec3(1./scale)); }
        glm::mat4 mvpMat = I;

        void update();

    public:
        Trackball () { reset(); }
        const GLfloat* mvpPtr() const { return glm::value_ptr(mvpMat); }
        glm::vec3 viewDirection() const { return viewDir; }

        void reset();
        void setDimensions(int, int);
        inline void getDimensions(uint& w, uint& h) const {
            w = width;
            h = height;
        }
        glm::vec3 getPoint(int, int) const;
        void dragged(int, int, int, int);
        void panning(int, int, int, int);


        // Scale calculation
        void zoom(float);
};

#endif