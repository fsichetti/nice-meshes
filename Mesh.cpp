#include "Mesh.hpp"
#include <iostream>

// Constructor
Mesh::Mesh(bool nrm, bool par, bool dif) :
    hasNrm(nrm), hasPar(par), hasDif(dif),
    attCnt(1 + nrm + par + dif),
    attCmp(3 + nrm*3 + par*2 + dif*2),
    attByt(attCmp*sizeof(GLfloat))
    {
};

// Reserve space in verts and faces arrays
void Mesh::reserveSpace(unsigned int rv, unsigned int re) {
    verts.reserve(rv * attCmp);
    faces.reserve(re * 3);
}


// Utility method to convert an attrib constant to an offset
unsigned int Mesh::attToOff(Attribute att) const {
    switch (att) {
        case X: return 0;
        case Y: return 1;
        case Z: return 2;

        case NX:
            if (!hasNrm) throw NoAttributeException();
            return 3;
        case NY:
            if (!hasNrm) throw NoAttributeException();
            return 4;
        case NZ:
            if (!hasNrm) throw NoAttributeException();
            return 5;

        case U:
            if (!hasPar) throw NoAttributeException();
            return 3 + 3*hasNrm;
        case V:
            if (!hasPar) throw NoAttributeException();
            return 4 + 3*hasNrm;

        case K:
            if (!hasDif) throw NoAttributeException();
            return 3 + 3*hasNrm + 2*hasPar;
        case H:
            if (!hasDif) throw NoAttributeException();
            return 4 + 3*hasNrm + 2*hasPar;
    }
    throw NoAttributeException();
}

unsigned int Mesh::addVertex() {
    for (int i=0; i<attCmp; ++i)
        verts.push_back(0);
    return vNum++;
}

unsigned int Mesh::addVertex(GLfloat x, GLfloat y, GLfloat z) {
    verts.insert(verts.end(), {x,y,z});
    // add padding for other attributes
    for (int i=3; i<attCmp; ++i)
        verts.push_back(0);
    return vNum++;
}

unsigned int Mesh::addFace(GLuint i, GLuint j, GLuint k) {
    faces.insert(faces.end(), {i,j,k});
    return fNum++;
}


void Mesh::draw(GLuint drawMode) const {
    if (!final) throw Mesh::NotFinalizedException();
    glBindVertexArray(vao);
    glDrawElements(drawMode, faces.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



// Prepare for drawing
void Mesh::finalize(bool nogui) {
    if (hasNrm && !normalsComputed) computeNormals();
    if (nogui) {
        final = true;
        return;
    }
    // Vertex array
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);

    // Vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        verts.size() * sizeof(GLfloat),
        verts.data(),
        GL_STATIC_DRAW
    );

    // Vertex array
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    size_t off = 0;   // attribute offset
    const size_t sf = sizeof(GLfloat);
    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, attByt, (GLvoid*)off);
    off += 3*sf;
    // Normals
    if (hasNrm) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, attByt, (GLvoid*)off);
        off += 3*sf;
    }
    // Parametric coords
    if (hasPar) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, false, attByt, (GLvoid*)off);
        off += 2*sf;
    }

    // Element buffer
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        faces.size() * sizeof(GLuint),
        faces.data(),
        GL_STATIC_DRAW
    );

    final = true;
    allocatedGLBuffers = true;
}



void Mesh::computeNormals(bool noCompute) {
    if (!hasNrm) throw NoAttributeException();
    normalsComputed = true;
    if (noCompute) return;

    // Compute normals
    auto normals = new glm::vec3[vNum];

    for (int i=0; i<vNum; ++i) {
        normals[i] = glm::vec3(0);
    }

    // for each face, compute normal
    for (int i=0; i<fNum; ++i) {
        glm::vec3 faceVert[3];
        // for each face vertex...
        for (int j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (int k=0; k<3; ++k) {
                faceVert[j][k] = cAttrib(faces[3*i+j], k);
            }
        }
        // Compute the cross product
        const glm::vec3 n = glm::cross(faceVert[2] - faceVert[0],
            faceVert[1] - faceVert[0]);
        // Accumulate unnormalised* normal on each vertex
        // *i.e. weighted by face area
        for (int j=0; j<3; ++j) {
            normals[faces[3*i+j]] += n;
        }
    }

    // for each vertex, normalise the normal and write to the vector
    for (int i=0; i<vNum; ++i) {
        normals[i] = -glm::normalize(normals[i]);
        attrib(i, Attribute::NX) = normals[i].x;
        attrib(i, Attribute::NY) = normals[i].y;
        attrib(i, Attribute::NZ) = normals[i].z;
    }
}


void Mesh::readOBJ(std::string path) {
    // Open file
    std::ifstream file(path);
    if (!file.is_open()) throw FileOpenException();

    // Get data
    while (!file.eof()) {
        std::string tok;
        file >> tok;
        // Ignores normals!
        if (tok.length() > 0) {
            std::string a, b, c;
            switch (tok[0]) {
                case 'v':
                    file >> a >> b >> c;
                    addVertex(std::stof(a), std::stof(b), std::stof(c));
                    break;

                case 'f':
                    file >> a >> b >> c;
                    addFace(std::stoi(a)-1, std::stoi(b)-1, std::stoi(c)-1);
                    break;

                case '#':
                    file.ignore(0xFF, '\n');
            }   
        }     
    }

    // Close file
    file.close();
}


void Mesh::writeOBJ(std::string path) const {
    if (!final) throw Mesh::NotFinalizedException();
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw FileOpenException();

    // Write verts
    for (int i=0; i < vNum; ++i) {
        file << "v "
            << cAttrib(i, Attribute::X) << " "
            << cAttrib(i, Attribute::Y) << " "
            << cAttrib(i, Attribute::Z) << std::endl;
    }
    // Write normals
    if (hasNrm) {
        for (int i=0; i < vNum; ++i) {
            file << "vn "
                << cAttrib(i, Attribute::NX) << " "
                << cAttrib(i, Attribute::NY) << " "
                << cAttrib(i, Attribute::NZ) << std::endl;
        }
    }
    // Write faces
    for (int i=0; i < faces.size(); i+=3) {
        file << "f "
            << faces[i+0] + 1 << " "
            << faces[i+1] + 1 << " "
            << faces[i+2] + 1 << std::endl;
    }

    // Close file
    file.close();
}

void Mesh::writePLY(std::string path) const {
    if (!final) throw Mesh::NotFinalizedException();
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw FileOpenException();
    
    // Header
    file << "ply" << std::endl << "format ascii 1.0" << std::endl;
    if (!name.empty()) file << "comment " << name << std::endl;
    file << "element vertex " << vNum << std::endl;
    file << "property float x" << std::endl << "property float y" <<
        std::endl << "property float z" << std::endl;
    if (hasNrm) file << "property float nx" << std::endl <<
        "property float ny" << std::endl << "property float nz" <<
        std::endl;
    if (hasPar) file << "property float u" << std::endl <<
        "property float v" << std::endl;
    if (hasDif) file << "property float k" << std::endl <<
        "property float h" << std::endl;
    file << "element face " << fNum << std::endl;
    file << "property list uchar int vertex_indices" << std::endl;
    file << "end_header" << std::endl;

    // Write vertices
    for (int i=0; i < verts.size(); i+=attCmp) {
        for (int j=0; j<attCmp; ++j) {
            file << verts[i+j] << " ";
        }
        file << std::endl;
    }

    // Write faces
    for (int i=0; i < faces.size(); i+=3) {
        file << "3 ";
        for (int j=0; j<3; ++j) {
            file << faces[i+j] << " ";
        }
        file << std::endl;
    }
    
    // Close file
    file.close();
}

void Mesh::writeOFF(std::string path) const {
    if (!final) throw Mesh::NotFinalizedException();
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw FileOpenException();

    // Header
    file << "OFF " << vNum << " " << fNum << " " << 0 << std::endl;

    // Write verts
    for (int i=0; i < vNum; ++i) {
        file << cAttrib(i, Attribute::X) << " "
            << cAttrib(i, Attribute::Y) << " "
            << cAttrib(i, Attribute::Z) << std::endl;
    }
    // Write faces
    for (int i=0; i < faces.size(); i+=3) {
        file << "3 "
            << faces[i+0] << " "
            << faces[i+1] << " "
            << faces[i+2] << std::endl;
    }

    // Close file
    file.close();
}


void Mesh::gaussNoise(float variance, bool nrm, bool tan) {
    if (!(nrm || tan)) return;
    for (unsigned int i = 0; i < vNum; ++i) {
        glm::vec3 v(
            cAttrib(i, Attribute::X),
            cAttrib(i, Attribute::Y),
            cAttrib(i, Attribute::Z)
        );
        glm::vec3 noise = RandPoint::gaussian3(variance);
        // IT'S NOT YET USING UNIFORM DISTRIBUTIONS FOR NRM AND TAN NOISES!!!
        if (nrm != tan) {
            if (!normalsComputed) computeNormals();
            glm::vec3 n(
                cAttrib(i, Attribute::NX),
                cAttrib(i, Attribute::NY),
                cAttrib(i, Attribute::NZ)
            );
            n = glm::normalize(n);
            n *= glm::dot(noise, n);    // Normal component of noise
            if (nrm) noise = n;
            else if (tan) noise -= n;
        }
        attrib(i, Attribute::X) += noise.x;
        attrib(i, Attribute::Y) += noise.y;
        attrib(i, Attribute::Z) += noise.z;
    }
}

void Mesh::makeCentered() {
    GLfloat com[3];
    for (unsigned int k = 0; k < 3; ++k) com[k] = 0;
    // Accumulate
    for (unsigned int i = 0; i < vNum; ++i) {
        com[0] += cAttrib(i, Attribute::X);
        com[1] += cAttrib(i, Attribute::Y);
        com[2] += cAttrib(i, Attribute::Z);
    }
    // Average
    for (unsigned int k = 0; k < 3; ++k) com[k] /= vNum;
    // Translate
    for (unsigned int i = 0; i < vNum; ++i) {
        attrib(i, Attribute::X) -= com[0];
        attrib(i, Attribute::Y) -= com[1];
        attrib(i, Attribute::Z) -= com[2];
    }
}


void Mesh::refine() {
    typedef unsigned long long int Edge;
    // Save edge and position within vertex list
    std::unordered_map<Edge, unsigned long int> edges;
    const unsigned long int oldFNum = fNum, oldVNum = vNum;
    edges.reserve(oldFNum * 3 / 2);
    for (unsigned long int fi = 0; fi < oldFNum; ++fi) {
        unsigned long int viOld[3], viNew[3];    // indices
        for (unsigned long int k = 0; k < 3; ++k) {
            viOld[k] = faces[3*fi + k];
        }
        // Get midpoints
        for (unsigned long int k = 0; k < 3; ++k) {
            // Indices of edge's endpoints
            const unsigned long int a = viOld[k], b = viOld[(k+1)%3];
            const Edge e = std::max(a,b) + oldVNum * std::min(a,b);
            // If already present
            if (edges.count(e) == 1) {
                viNew[k] = edges.at(e);
            }
            else {
                // Make new vertex
                viNew[k] = addVertex();
                // Average all attributes
                for (unsigned long int att = 0; att < attCnt; ++att) {
                    attrib(viNew[k], att) = (cAttrib(viOld[k], att) + 
                        cAttrib(viOld[(k+1)%3], att)) / 2;
                }
                edges.emplace(e, viNew[k]);                 // cache
            }
        }

        // Change the middle face
        for (unsigned long int k = 0; k < 3; ++k) {
            faces[3*fi + k] = viNew[k];
        }
        // Add 3 new faces
        addFace(viOld[0], viNew[0], viNew[2]);
        addFace(viOld[1], viNew[1], viNew[0]);
        addFace(viOld[2], viNew[2], viNew[1]);
    }
}


// cache this
float Mesh::avgEdgeLength() const {
    double len = 0;
    unsigned int cnt = 0;
    glm::vec3 faceVert[3];

    // for each face...
    for (int i=0; i<fNum; ++i) {
        // for each face vertex...
        for (int j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (int k=0; k<3; ++k) {
                faceVert[j][k] = cAttrib(faces[3*i+j], k);
            }
        }
        len += glm::length(faceVert[0] - faceVert[1]);
        len += glm::length(faceVert[1] - faceVert[2]);
        len += glm::length(faceVert[2] - faceVert[0]);
        cnt += 3;
    }
    return len / static_cast<double>(cnt);
}

// cache this
float Mesh::getVolume() const {
    float vol = 0;
    glm::vec3 faceVert[3];
    glm::vec3 nA;

    // for each face...
    for (int i=0; i<fNum; ++i) {
        // for each face vertex...
        for (int j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (int k=0; k<3; ++k) {
                faceVert[j][k] = cAttrib(faces[3*i+j], k);
            }
        }
        // face normal * 2 * face area
        nA = glm::cross(faceVert[1] - faceVert[0], faceVert[2] - faceVert[0]);
        for (int j=0; j<3; ++j) {
            // dot and accumulate
            vol += glm::dot(faceVert[0], nA);
        }
    }
    return vol * 3 / 2;
}