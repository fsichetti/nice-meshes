#include "VectorField.hpp"

VectorField::VectorField(Mesh* m, bool onFaces) :
    mesh(m), samples(onFaces ? m->faceNum() : m->vertNum()) {
    for (int i = 0; i < 3; ++i) {
		components[i] = new ScalarField(m, 0, onFaces);
	}
}

VectorField::~VectorField() {
    for (int i = 0; i < 3; ++i) {
		delete components[i];
	}
}

void VectorField::setValue(glm::dvec3 value, uint index) {
	components[0]->setValue(value.x, index);
	components[1]->setValue(value.y, index);
	components[2]->setValue(value.z, index);
}

glm::dvec3 VectorField::getValue(uint index) const {
	return glm::dvec3(
		components[0]->getValue(index),
		components[1]->getValue(index),
		components[2]->getValue(index)
	);
}

void VectorField::write(std::string path, bool header) const {
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw Mesh::FileOpenException();

    // Write header
    if (header) {
        file << "VECTOR_FIELD " << samples << std::endl;
    }
    // Write values
    for (uint i = 0; i < samples; ++i) {
		const glm::dvec3 v = getValue(i);
        file << std::setprecision(DPRECIS) << v.x << " "
            << std::setprecision(DPRECIS) << v.y << " "
            << std::setprecision(DPRECIS) << v.z << std::endl;
    }

    // Close file
    file.close();
}

void VectorField::write2d(std::string path) const {
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw Mesh::FileOpenException();

    // Write verts
    for (uint i = 0; i < samples; ++i) {
		const glm::dvec3 v = getValue(i);
        file << std::setprecision(DPRECIS) << v.x << " "
            << std::setprecision(DPRECIS) << v.y << std::endl;
    }

    // Close file
    file.close();
}