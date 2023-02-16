#ifndef SCALARFIELD_H
#define SCALARFIELD_H

#include "Mesh.hpp"

class ScalarField {
    public:
        ScalarField(Mesh* m, unsigned int derivatives = 0);
        ~ScalarField();
        void setValue(double value, unsigned int i, unsigned int uDeriv = 0,
            unsigned int vDeriv = 0);
        double getValue(unsigned int i, unsigned int uDeriv = 0,
            unsigned int vDeriv = 0) const;
        void write(std::string path, bool header=false) const;
        class TooManyValuesException;
    private:
        const Mesh* mesh;
        const unsigned int samples, deriv;
        double* values;
        unsigned int pair(unsigned int x, unsigned int y) const;
};

class ScalarField::TooManyValuesException : public std::exception {
    public: const char* what() { return "Inserting too many values"; }
};

#endif