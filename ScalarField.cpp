#include "ScalarField.hpp"

ScalarField::ScalarField(Mesh* m) :
    mesh(m), samples(m->vertNum()) {
    values = new float[samples];
}

ScalarField::~ScalarField() {
    delete values;
}

void ScalarField::addValue(float val) {
    if (counter >= samples) { throw TooManyValuesException(); }
    values[counter++] = val;    
}

void ScalarField::write(std::string path) const {
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw Mesh::FileOpenException();

    // Write verts
    for (unsigned int i = 0; i < samples; ++i) {
        file << values[i] << std::endl;
    }

    // Close file
    file.close();
}