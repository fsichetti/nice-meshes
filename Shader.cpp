#include "Shader.hpp"

Shader::Shader(std::string vertex, std::string fragment) {
    GLuint vert = load(vertex, GL_VERTEX_SHADER);
    GLuint frag = load(fragment, GL_FRAGMENT_SHADER);
    link();
    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::Shader(std::string vertex, std::string geometry, std::string fragment) {
    GLuint vert = load(vertex, GL_VERTEX_SHADER);
    GLuint geom = load(geometry, GL_GEOMETRY_SHADER);
    GLuint frag = load(fragment, GL_FRAGMENT_SHADER);
    link();
    glDeleteShader(vert);
    glDeleteShader(geom);
    glDeleteShader(frag);
}

GLuint Shader::load(std::string filename, GLenum shdType) {
    GLuint id;
    std::string shdString;
    std::ifstream srcStream(filename);
    if (srcStream) {
        shdString.assign(
            std::istreambuf_iterator<char>(srcStream), // start at beginning
            std::istreambuf_iterator<char>()     // stop when null
        );

        id = glCreateShader(shdType);
        const GLchar* shdSource = shdString.c_str();
        glShaderSource(id, 1, &shdSource, NULL);
        glCompileShader(id);
        checkCompileErrors(id, filename);
        glAttachShader(program, id);
    }

    return id;
}

void Shader::link() {
    glLinkProgram(program);
    checkLinkErrors(program);
}


bool Shader::checkCompileErrors(GLint ShaderId, std::string fname) {
    GLint ok = -1;
    glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        const int max_length = 2048;
        int length = 0;
        char log[2048];
        glGetShaderInfoLog(ShaderId, max_length, &length, log);
        std::cerr << "Shader " << ShaderId << " (" << fname << ") "
            "error log: " << log << std::endl;
        return false;
    }
    return true;
}

bool Shader::checkLinkErrors(GLint program) {
    GLint ok = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        const int max_length = 2048;
        int length = 0;
        char log[2048];
        glGetProgramInfoLog(program, max_length, &length, log);
        std::cerr << "Program " << program <<
            " error log: " << log << std::endl;
        return false;
    }
    return true;
}