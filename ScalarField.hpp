#ifndef SCALARFIELD_H
#define SCALARFIELD_H

#include "Mesh.hpp"

class ScalarField {
    public:
        ScalarField(Mesh* m, uint derivatives = 0, bool onFaces=false);
        ~ScalarField();
        void setValue(double value, uint i, uint uDeriv = 0,
            uint vDeriv = 0);
        double getValue(uint i, uint uDeriv = 0,
            uint vDeriv = 0) const;
        void write(std::string path, bool header=false) const;
        class TooManyValuesException;
    private:
        const Mesh* mesh;
        const uint samples, deriv;
        double* values;
        uint pair(uint x, uint y) const;
};

class ScalarField::TooManyValuesException : public std::exception {
    public: const char* what() { return "Inserting too many values"; }
};

#endif