#include "ScalarField.hpp"

ScalarField::ScalarField(Mesh* m, uint d, bool onFaces) :
    mesh(m), samples(onFaces ? m->faceNum() : m->vertNum()),
    deriv((d+2) * (d+1) / 2) {
    values = new double[samples * deriv];
}

ScalarField::~ScalarField() {
    delete values;
}

void ScalarField::setValue(double val, uint i,
    uint du, uint dv) {
    // Cantor pairing function
    uint index = i * deriv + pair(du, dv);
    if (index >= samples * deriv) { throw TooManyValuesException(); }
    values[index] = val;
}

double ScalarField::getValue(uint i, uint du,
    uint dv) const {
    uint index = i * deriv + pair(du, dv);
    return values[index];
}

void ScalarField::write(std::string path, bool header) const {
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw Mesh::FileOpenException();

    // Write header
    if (header) {
        file << "SCALAR_FIELD " << samples << std::endl;
    }
    // Write values
    for (uint i = 0; i < samples; ++i) {
        file << getValue(i) << std::endl;
    }

    // Close file
    file.close();
}

uint ScalarField::pair(uint x, uint y) const {
    return (x*x + 3*x + 2*x*y + y + y*y) / 2;
}