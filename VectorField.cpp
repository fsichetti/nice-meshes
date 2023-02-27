#include "VectorField.hpp"

VectorField::VectorField(Mesh* m) :
    mesh(m), samples(m->vertNum()) {
    for (int i = 0; i < 3; ++i) {
		components[i] = new ScalarField(m, 0);
	}
}

VectorField::~VectorField() {
    for (int i = 0; i < 3; ++i) {
		delete components[i];
	}
}

void VectorField::setValue(glm::vec3 value, unsigned int index) {
	components[0]->setValue(value.x, index);
	components[1]->setValue(value.y, index);
	components[2]->setValue(value.z, index);
}

glm::vec3 VectorField::getValue(unsigned int index) const {
	return glm::vec3(
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
        file << "VECTOR_FIELD " << mesh->vertNum() << std::endl;
    }
    // Write verts
    for (unsigned int i = 0; i < samples; ++i) {
		const glm::vec3 v = getValue(i);
        file << v.x << " " << v.y << " " << v.z << std::endl;
    }

    // Close file
    file.close();
}

void VectorField::write2d(std::string path) const {
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw Mesh::FileOpenException();

    // Write verts
    for (unsigned int i = 0; i < samples; ++i) {
		const glm::vec3 v = getValue(i);
        file << v.x << " " << v.y << std::endl;
    }

    // Close file
    file.close();
}