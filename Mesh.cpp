#include "Mesh.hpp"

// Constructor
Mesh::Mesh(bool nrm, bool par, bool dif) :
    hasNrm(nrm), hasPar(par), hasDif(dif),
    attCnt(1 + nrm + par + dif),
    attCmp(3 + nrm*3 + par*2 + dif*2)
    {};

// Reserve space in verts and faces arrays
void Mesh::reserveSpace(uint nv, uint nf) {
    verts.reserve(nv * attCmp);
    faces.reserve(nf* 3);
}

DifferentialQuantities Mesh::diffEvaluate(double u, double v) const {
    throw NotFinalizedException();
}


// Utility method to convert an attrib constant to an offset
uint Mesh::attToOff(Attribute att) const {
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

uint Mesh::addVertex() {
    for (uint i=0; i<attCmp; ++i)
        verts.push_back(0);
    return vNum++;
}

uint Mesh::addVertex(double x, double y, double z) {
    verts.insert(verts.end(), {x,y,z});
    // add padding for other attributes
    for (uint i=3; i<attCmp; ++i)
        verts.push_back(0);
    return vNum++;
}

uint Mesh::addFace(uint i, uint j, uint k) {
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
    // Generate GL data arrays
    std::vector<GLfloat> gpuVerts(verts.begin(), verts.end());
    std::vector<GLuint> gpuFaces(faces.begin(), faces.end());

    // Vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        gpuVerts.size() * sizeof(GLfloat),
        gpuVerts.data(),
        GL_STATIC_DRAW
    );

    // Vertex array
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    size_t off = 0;   // attribute offset
    const size_t sf = sizeof(GLfloat);
    const size_t attByt = attCmp * sf;
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
        gpuFaces.size() * sizeof(GLuint),
        gpuFaces.data(),
        GL_STATIC_DRAW
    );

    final = true;
    allocatedGLBuffers = true;
}

Mesh::~Mesh() {
    deleteBuffers();
}

void Mesh::deleteBuffers()  {
    if (!allocatedGLBuffers) return;
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    vbo = 0;
    ebo = 0;
    vao = 0;
    allocatedGLBuffers = false;
    final = false;  // may be unnecessary
}



void Mesh::computeNormals(bool noCompute) {
    if (!hasNrm) throw NoAttributeException();
    normalsComputed = true;
    if (noCompute) return;

    // Compute normals
    auto normals = new glm::dvec3[vNum];

    for (uint i=0; i<vNum; ++i) {
        normals[i] = glm::dvec3(0);
    }

    // for each face, compute normal
    for (uint i=0; i<fNum; ++i) {
        glm::dvec3 faceVert[3];
        // for each face vertex...
        for (uint j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (uint k=0; k<3; ++k) {
                faceVert[j][k] = cAttrib(faces[3*i+j], k);
            }
        }
        // Compute the cross product
        const glm::dvec3 n = glm::cross(faceVert[2] - faceVert[0],
            faceVert[1] - faceVert[0]);
        // Accumulate unnormalised* normal on each vertex
        // *i.e. weighted by face area
        for (uint j=0; j<3; ++j) {
            normals[faces[3*i+j]] += n;
        }
    }

    // for each vertex, normalise the normal and write to the vector
    for (uint i=0; i<vNum; ++i) {
        normals[i] = -glm::normalize(normals[i]);
        attrib(i, Attribute::NX) = normals[i].x;
        attrib(i, Attribute::NY) = normals[i].y;
        attrib(i, Attribute::NZ) = normals[i].z;
    }

    delete[] normals;
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
    for (uint i=0; i < vNum; ++i) {
        file << "v "
            << std::setprecision(DPRECIS) << cAttrib(i, Attribute::X) << " "
            << std::setprecision(DPRECIS) << cAttrib(i, Attribute::Y) << " "
            << std::setprecision(DPRECIS) << cAttrib(i, Attribute::Z)
            << std::endl;
    }
    // Write normals
    if (hasNrm) {
        for (uint i=0; i < vNum; ++i) {
            file << "vn "
                << std::setprecision(DPRECIS) << cAttrib(i, Attribute::NX) << " "
                << std::setprecision(DPRECIS) << cAttrib(i, Attribute::NY) << " "
                << std::setprecision(DPRECIS) << cAttrib(i, Attribute::NZ)
                << std::endl;
        }
    }
    // Write faces
    for (uint i=0; i < faces.size(); i+=3) {
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
    file << "property double x" << std::endl << "property double y" <<
        std::endl << "property double z" << std::endl;
    if (hasNrm) file << "property double nx" << std::endl <<
        "property double ny" << std::endl << "property double nz" <<
        std::endl;
    if (hasPar) file << "property double u" << std::endl <<
        "property double v" << std::endl;
    if (hasDif) file << "property double k" << std::endl <<
        "property double h" << std::endl;
    file << "element face " << fNum << std::endl;
    file << "property list uchar int vertex_indices" << std::endl;
    file << "end_header" << std::endl;

    // Write vertices
    for (uint i=0; i < verts.size(); i+=attCmp) {
        for (uint j=0; j<attCmp; ++j) {
            file << std::setprecision(DPRECIS) << verts[i+j] << " ";
        }
        file << std::endl;
    }

    // Write faces
    for (uint i=0; i < faces.size(); i+=3) {
        file << "3 ";
        for (uint j=0; j<3; ++j) {
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
    for (uint i=0; i < vNum; ++i) {
        file << std::setprecision(DPRECIS) << cAttrib(i, Attribute::X) << " "
            << std::setprecision(DPRECIS) << cAttrib(i, Attribute::Y) << " "
            << std::setprecision(DPRECIS) << cAttrib(i, Attribute::Z) << std::endl;
    }
    // Write faces
    for (uint i=0; i < faces.size(); i+=3) {
        file << "3 "
            << faces[i+0] << " "
            << faces[i+1] << " "
            << faces[i+2] << std::endl;
    }

    // Close file
    file.close();
}


void Mesh::gaussNoise(double variance, bool nrm, bool tan) {
    if (!(nrm || tan)) return;
    for (uint i = 0; i < vNum; ++i) {
        glm::dvec3 noise(0);
        if (nrm && tan) {
            noise = RandPoint::gaussian3(variance);
        }
        else {
            if (!normalsComputed) computeNormals();
            glm::dvec3 normalVec(
                cAttrib(i, Attribute::NX),
                cAttrib(i, Attribute::NY),
                cAttrib(i, Attribute::NZ)
            );
            normalVec = glm::normalize(normalVec);
            if (tan) {
                // Construct a basis for the tangent space
                glm::dvec3 v1, v2;
                if (normalVec.z != 0) {
                    v1 = glm::dvec3(1, 1, 
                        -(normalVec.x + normalVec.y)/normalVec.z);
                    v1 = glm::normalize(v1);
                }
                else v1 = glm::dvec3(0,0,1);
                v2 = glm::cross(normalVec, v1);
                v2 = glm::normalize(v2);
                const auto n = RandPoint::gaussian2(variance);
                noise = v1 * n.s + v2 * n.t;
            }
            if (nrm) {
                noise = normalVec * RandPoint::gaussian1(variance);
            }
        }

        attrib(i, Attribute::X) += noise.x;
        attrib(i, Attribute::Y) += noise.y;
        attrib(i, Attribute::Z) += noise.z;
    }
}

void Mesh::makeCentered() {
    double com[3];
    for (uint k = 0; k < 3; ++k) com[k] = 0;
    // Accumulate
    for (uint i = 0; i < vNum; ++i) {
        com[0] += cAttrib(i, Attribute::X);
        com[1] += cAttrib(i, Attribute::Y);
        com[2] += cAttrib(i, Attribute::Z);
    }
    // Average
    for (uint k = 0; k < 3; ++k) com[k] /= vNum;
    // Translate
    for (uint i = 0; i < vNum; ++i) {
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
double Mesh::getAverageEdgeLength() const {
    double len = 0;
    uint cnt = 0;
    glm::dvec3 faceVert[3];

    // for each face...
    for (uint i=0; i<fNum; ++i) {
        // for each face vertex...
        for (uint j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (uint k=0; k<3; ++k) {
                faceVert[j][k] = cAttrib(cFacei(i,j), k);
            }
        }
        len += glm::length(faceVert[0] - faceVert[1]);
        len += glm::length(faceVert[1] - faceVert[2]);
        len += glm::length(faceVert[2] - faceVert[0]);
        cnt += 3;
    }
    const double res = len / static_cast<double>(cnt);
    return res;
}

// cache this
double Mesh::getVolume() const {
    double vol = 0;
    glm::dvec3 faceVert[3];
    glm::dvec3 nA;

    // for each face...
    for (uint i=0; i<fNum; ++i) {
        // for each face vertex...
        for (uint j=0; j<3; ++j) {
            // for each xyz component, retrieve value
            for (uint k=0; k<3; ++k) {
                faceVert[j][k] = cAttrib(faces[3*i+j], k);
            }
        }
        // face normal * 2 * face area
        nA = glm::cross(faceVert[1] - faceVert[0], faceVert[2] - faceVert[0]);
        for (uint j=0; j<3; ++j) {
            // dot and accumulate
            vol += glm::dot(faceVert[0], nA);
        }
    }
    const double res = vol * 3 / 2;
    return res;
}

double Mesh::getArea(uint faceId) const {
    glm::dvec3 v[3];
    // for each face vertex...
    for (uint j=0; j<3; ++j) {
        // for each xyz component, retrieve value
        for (uint k=0; k<3; ++k) {
            v[j][k] = cAttrib(cFacei(faceId,j), k);
        }
    }
    const glm::dvec3 nA = glm::cross(v[1] - v[0], v[2] - v[0]);
    return glm::length(nA) / 2;
}


glm::dvec2 Mesh::randomPointUV() {
    // Compute CDF if necessary
    if (faceCDF.size() == 0) {
        double sum = 0;
        faceCDF.reserve(faceNum());
        for (uint i = 0; i < faceNum(); ++i) {
            sum += getArea(i);
            faceCDF.push_back(sum);
        }
    }
    const double r = glm::linearRand(0., faceCDF.back());
    uint randomFace = 0;
    while (r > faceCDF.at(randomFace)) ++randomFace;
    // Get face
    glm::dvec2 v[3];
    for (uint j=0; j<3; ++j) {
        v[j].x = cAttrib(cFacei(randomFace,j), Attribute::U);
        v[j].y = cAttrib(cFacei(randomFace,j), Attribute::V);
        // Check if the triangle overlaps the border of the uv plane
        // Very reasonable assumption: no triangle spans more than 
        // half the plane in U or V direction
        const double lmax = .25, rmin = .75;
        // If there are any points on the far right...
        if (v[0].x > rmin || v[1].x > rmin || v[2].x > rmin) {
            // Check which points lie on the far left and "unwrap" them
            for (uint k = 0; k < 3; ++k) {
                if (v[k].x < lmax) v[k].x += 1;
            }
        }
        // Same for the y
        if (v[0].y > rmin || v[1].y > rmin || v[2].y > rmin) {
            for (uint k = 0; k < 3; ++k) {
                if (v[k].y < lmax) v[k].y += 1;
            }
        }
    }
    auto rnd = RandPoint::inTriangle(v[0], v[1], v[2]);
    // Wrap back the coordinates for triangles over the border
    if (rnd.x > 1) rnd.x -= 1;
    if (rnd.y > 1) rnd.y -= 1;
    return rnd;
}

std::vector<glm::dvec2> Mesh::uniformSampling(uint samples, bool corners,
    uint border) {
    std::vector<glm::dvec2> newVerts;
    if (samples < 4) corners = false;    // extreme case
    newVerts.reserve(samples);
    // Add corner vertices first
    if (corners) {
        newVerts.push_back(glm::dvec2(0,0));
        newVerts.push_back(glm::dvec2(1,0));
        newVerts.push_back(glm::dvec2(1,1));
        newVerts.push_back(glm::dvec2(0,1));
    }
    // Then, add border vertices (in groups of 4)
    const uint start = newVerts.size();
    for (uint i = newVerts.size(); i < border; i+=4) {
        const auto rand = randomPointUV();
        newVerts.push_back(glm::dvec2(rand.x,0));
        newVerts.push_back(glm::dvec2(1,rand.y));
        newVerts.push_back(glm::dvec2(rand.x,1));
        newVerts.push_back(glm::dvec2(0,rand.y));
    }
    // Finally, add inner vertices
    for (uint i = newVerts.size(); i < samples; ++i)
        newVerts.push_back(randomPointUV());
    return newVerts;
}