#ifndef BROWSER_H
#define BROWSER_H

#include <epoxy/gl.h>
#include <GL/freeglut.h>
#include <FreeImage.h>
#include <iostream>
#include <string>
#include <stdlib.h>

#include "Shader.hpp"
#include "Trackball.hpp"
#include "Mesh.hpp"

// NOTE: Revert to class eventually?
namespace Browser {
    void init(char*, Trackball*);
    void setMesh(Mesh*);
    void setOutPath(std::string);
    void launch();

    void callbackDisplay();
    void callbackSpecial(int, int, int);
    void callbackKeyboard(unsigned char, int, int);
    void callbackMouse(int, int, int, int);
    void callbackMotion(int, int);
    void callbackReshape(int, int);
    void callbackClose();

    void screenshot(std::string filename);
    // void message(float x, float y, std::string msg);
};

#endif