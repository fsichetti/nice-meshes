#include "Trackball.hpp"

void Trackball::update() {
    glm::mat4 t = glm::translate(I, pan);
    glm::mat4 r = glm::toMat4(rot);
    glm::mat4 s = glm::scale(I, glm::vec3(scale));
    viewDir = glm::transpose(r) * glm::vec4(0.0, 0.0, 1.0, 0.0);
    mvpMat = pMat * t * r * s;
    mvpMat = glm::transpose(mvpMat);
}

void Trackball::setDimensions(int w, int h) {
    width = w;
    height = h;
    pMat = glm::perspective(fov, (GLfloat)w/(GLfloat)h, near, far);
    update();
}

void Trackball::zoom(float delta) {
    scaleParameter += delta;
    scale = (scaleParameter < 0) ? 
        glm::exp2(scaleParameter) : 
        glm::log2(scaleParameter + 1) + 1;
    update();
}

glm::vec3 Trackball::getPoint(int x, int y) {
    glm::vec3 point;
    point.x = x - (GLfloat)width/2;
    point.y = y - (GLfloat)height/2;

    float n2 = glm::pow(point.x, 2) + glm::pow(point.y, 2);
    point.z = (n2 <= r2/2) ?
        glm::sqrt(r2-n2) :
        r2 / (2 * glm::sqrt(n2));
    
    point = glm::normalize(point);
    return point;
}

void Trackball::dragged(int x0, int y0, int x1, int y1) {
    glm::vec3 p0 = getPoint(x0, y0);
    glm::vec3 p1 = getPoint(x1, y1);
    
    glm::vec3 axis = glm::cross(p0, p1);
    auto eq = glm::epsilonEqual(p0, p1, 0.0001f);
    if (eq[0]||eq[1]||eq[2]) return;

    axis.x = -axis.x;
    axis.z = -axis.z;
    axis = glm::inverse(rot) * axis;
    axis = glm::normalize(axis);
    GLfloat dot = glm::dot(p0, p1);
    GLfloat angle = glm::acos(dot);

    glm::quat qr = glm::angleAxis(angle, axis);
    qr = glm::normalize(qr);
    rot = rot * qr;

    update();
}

void Trackball::panning(int x0, int y0, int x1, int y1) {
    glm::vec3 d(x1-x0, y0-y1, 0);
    pan += glm::vec1(.01)*d;

    update();
}