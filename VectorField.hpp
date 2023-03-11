#ifndef VECTORFIELD_H
#define VECTORFIELD_H

#include "ScalarField.hpp"
#include <glm/glm.hpp>

class VectorField {
    public:
        VectorField(Mesh* m, bool onFaces=false);
        ~VectorField();
        void setValue(glm::dvec3 value, uint i);
        glm::dvec3 getValue(uint i) const;
        void write(std::string path, bool header=false) const;
        void write2d(std::string path) const;

    private:
        const Mesh* mesh;
        const uint samples;
        ScalarField* components[3];
        uint pair(uint x, uint y) const;
};

#endif