#ifndef SCALARFIELD_H
#define SCALARFIELD_H

#include "Mesh.hpp"

class ScalarField {
    public:
        ScalarField(Mesh* m);
        ~ScalarField();
        void addValue(float val);
        float getValue(unsigned int i) const;
        void write(std::string path, bool header=false) const;
        class TooManyValuesException;
    private:
        const Mesh* mesh;
        const unsigned int samples;
        float* values;

        unsigned int counter = 0;
};

class ScalarField::TooManyValuesException : public std::exception {
    public: const char* what() { return "Inserting too many values"; }
};

#endif