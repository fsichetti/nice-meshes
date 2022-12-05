#ifndef SHDFUNC_H
#define SHDFUNC_H

#include <string>
#include <fstream>
#include <iostream>
#include <epoxy/gl.h>
#include <epoxy/glx.h>

class Shader {
    public:
    Shader(std::string, std::string);
    Shader(std::string, std::string, std::string);
    operator GLuint() { return program; };

    class CompileException;
    class LinkException;

    private:
    const GLuint program = glCreateProgram();
    GLuint load(std::string, GLenum);
    void link();
    bool checkCompileErrors(GLint, std::string="");
    bool checkLinkErrors(GLint);
};

class Shader::CompileException : public std::exception {
    public: const char* what() { return "Could not compile shader"; }
};

class Shader::LinkException : public std::exception {
    public: const char* what() { return "Could not link program"; }
};

#endif