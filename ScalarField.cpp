#include "ScalarField.hpp"

ScalarField::ScalarField(Mesh* m, unsigned int d) :
    mesh(m), samples(m->vertNum()), deriv((d+2) * (d+1) / 2) {
    values = new double[samples * deriv];
}

ScalarField::~ScalarField() {
    delete values;
}

void ScalarField::setValue(double val, unsigned int i,
    unsigned int du, unsigned int dv) {
    // Cantor pairing function
    unsigned int index = i * deriv + pair(du, dv);
    if (index >= samples * deriv) { throw TooManyValuesException(); }
    values[index] = val;
}

double ScalarField::getValue(unsigned int i, unsigned int du,
    unsigned int dv) const {
    unsigned int index = i * deriv + pair(du, dv);
    return values[index];
}

void ScalarField::write(std::string path, bool header) const {
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw Mesh::FileOpenException();

    // Write header
    if (header) {
        file << "SCALAR_FIELD " << mesh->vertNum() << std::endl;
    }
    // Write verts
    for (unsigned int i = 0; i < samples; ++i) {
        file << getValue(i) << std::endl;
    }

    // Close file
    file.close();
}

unsigned int ScalarField::pair(unsigned int x, unsigned int y) const {
    return (x*x + 3*x + 2*x*y + y + y*y) / 2;
}