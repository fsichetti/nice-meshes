#include "Mesh.hpp"

// Constructor
Mesh::Mesh(bool nrm, bool par) :
    hasNrm(nrm), hasPar(par),
    attCnt(1 + nrm + par),
    attCmp(3 + nrm*3 + par*2),
    attByt(attCmp*sizeof(GLfloat))
    {
};

// Destructor
Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

// Reserve space in vertices and faces arrays
void Mesh::reserveSpace(unsigned int rv, unsigned int re) {
    vertices.reserve(rv * attCmp);
    faces.reserve(re * 3);
}


unsigned int Mesh::addVertex(GLfloat x, GLfloat y, GLfloat z) {
    vertices.insert(vertices.end(), {x,y,z});
    // add padding for other attributes
    for (int i=3; i<attCmp; ++i)
        vertices.push_back(0);
    return ++vNum;
}

unsigned int Mesh::addFace(GLuint i, GLuint j, GLuint k){
    faces.insert(faces.end(), {i,j,k});
    return ++fNum;
}

GLfloat* Mesh::vComponent(int vertex_index, int attrib_offset) {
    return &vertices[vertex_index + attCmp * attrib_offset];
}


void Mesh::draw(GLuint drawMode) const {
    if (!final) return;
    glBindVertexArray(vao);
    glDrawElements(drawMode, faces.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



// Prepare for drawing
void Mesh::finalize() {
    if (hasNrm) computeNormals();

    // Vertex array
    glBindVertexArray(vao);

    // Vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // Vertex array
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int cnt = 0;   // attribute counter
    size_t off = 0;   // attribute offset
    const size_t sf = sizeof(GLfloat);
    // Positions
    glEnableVertexAttribArray(cnt);
    glVertexAttribPointer(cnt, 3, GL_FLOAT, false, attByt, (GLvoid*)off);
    off += 3*sf;
    // Normals
    if (hasNrm) {
        glEnableVertexAttribArray(++cnt);
        glVertexAttribPointer(cnt, 3, GL_FLOAT, false, attByt, (GLvoid*)off);
        off += 3*sf;
    }
    // Parametric coords
    if (hasPar) {
        glEnableVertexAttribArray(++cnt);
        glVertexAttribPointer(cnt, 2, GL_FLOAT, false, attByt, (GLvoid*)off);
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
}



void Mesh::computeNormals() {
    // Compute normals
    std::vector<glm::vec3> faceVert(3);     // used for normals computation
    std::vector<glm::vec3> normals(vNum);     // used for normals computation
    glm::vec3 n;

    for (int i=0; i<vNum; ++i) {
        normals.push_back(glm::vec3(0));
    }

    // for each face, compute normal
    for (int i=0; i<fNum; ++i) {
        // for each face vertex...
        for (int j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (int k=0; k<3; ++k) {
                faceVert[j][k] = vertices[attCmp*faces[3*i+j]+k];
            }
        }
        // Compute the cross product
        n = glm::cross(faceVert[1] - faceVert[0], faceVert[2] - faceVert[0]);
        // Accumulate unnormalised* normal on each vertex
        // *i.e. weighted by face area
        for (int j=0; j<3; ++j) {
            normals[faces[3*i+j]] += n;
        }
    }

    // for each vertex, normalise the normal and write to the vector
    for (int i=0; i<vNum; ++i) {
        normals[i] = glm::normalize(normals[i]);
        for (int k=0; k<3; ++k) {
            vertices[attCmp*i+k+3] = normals[i][k];
        }
    }
}



void Mesh::writeToObj(std::string path) const {
    if (!final) return;
    // Open file
    std::ofstream file(path);
    if (file.is_open()) {
        // Write vertices
        for (int i=0; i < vertices.size(); i+=attCmp) {
            file << "v "
                << vertices[i+0] << " "
                << vertices[i+1] << " "
                << vertices[i+2] << " "
                << std::endl;
        }
        // Write normals
        if (hasNrm) {
            for (int i=0; i < vertices.size(); i+=attCmp) {
                file << "vn "
                    << vertices[i+3] << " "
                    << vertices[i+4] << " "
                    << vertices[i+5] << " "
                    << std::endl;
            }
        }
        // Write faces
        for (int i=0; i < faces.size(); i+=3) {
            file << "f "
                << faces[i+0] + 1 << " "
                << faces[i+1] + 1 << " "
                << faces[i+2] + 1 << " "
                << std::endl;
        }
    } 
    else {
        std::cerr << "Could not open file " << path << std::endl;
    }
    // Close file
    file.close();
}

void Mesh::writeToPly(std::string path) const {
    if (!final) return;
    // Open file
    std::ofstream file(path);
    if (file.is_open()) {
        // Header
        file << "ply\r" << "format ascii 1.0\r";
        if (!name.empty()) file << "comment " << name << "\r";
        file << "face vertex " << vNum << "\r";
        file << "property float x\r" << "property float y\r"
            << "property float z\r";
        if (hasNrm) file << "property float nx\r" << "property float ny\r"
            << "property float nz\r";
        if (hasPar) file << "property float u\r" << "property float v\r";
        file << "face face " << fNum << "\r";
        file << "property list uchar int vertex_indices\r";
        file << "end_header\r";

        // Write vertices
        for (int i=0; i < vertices.size(); i+=attCmp) {
            for (int j=0; j<attCmp; ++j) {
                file << vertices.at(i+j) << " ";
            }
            file << "\r";
        }

        // Write faces
        for (int i=0; i < faces.size(); i+=3) {
            file << "3 ";
            for (int j=0; j<3; ++j) {
                file << faces.at(i+j) << " ";
            }
            file << "\r";
        }
    } 
    else {
        std::cerr << "Could not open file " << path << std::endl;
    }
    // Close file
    file.close();
}

float Mesh::getVolume() const {
    float vol = 0;
    std::vector<glm::vec3> faceVert(3);
    glm::vec3 nA;

    // for each face...
    for (int i=0; i<fNum; ++i) {
        // for each face vertex...
        for (int j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (int k=0; k<3; ++k) {
                faceVert[j][k] = vertices[attCmp*faces[3*i+j]+k];
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