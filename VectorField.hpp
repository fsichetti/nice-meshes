#ifndef VECTORFIELD_H
#define VECTORFIELD_H

#include "ScalarField.hpp"
#include <glm/glm.hpp>

class VectorField {
    public:
        VectorField(Mesh* m, bool onFaces=false);
        ~VectorField();
        void setValue(glm::vec3 value, unsigned int i);
        glm::vec3 getValue(unsigned int i) const;
        void write(std::string path, bool header=false) const;
        void write2d(std::string path) const;

    private:
        const Mesh* mesh;
        const unsigned int samples;
        ScalarField* components[3];
        unsigned int pair(unsigned int x, unsigned int y) const;
};

#endif