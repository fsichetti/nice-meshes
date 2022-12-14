#ifndef MESH_H
#define MESH_H

#include <vector>
#include <fstream>
#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include "RandPoint.hpp"

class Mesh { 
    public:
        typedef std::vector<GLfloat> vArray;
        typedef std::vector<GLuint> fArray;

        std::string name = "";

        // Init mesh with expected number of vertices and faces
        Mesh(bool normals, bool parametric);
        ~Mesh();

        // Core methods
        void reserveSpace(unsigned int verts, unsigned int faces);
        unsigned int addVertex(GLfloat x, GLfloat y, GLfloat z);
        unsigned int addFace(GLuint i, GLuint j, GLuint k);
        void finalize(bool nogui = false);
        void draw(GLuint drawMode = GL_TRIANGLES) const;
        void writeToObj(std::string filename) const;
        void writeToPly(std::string filename) const;

        // Access methods
        enum Attribute { X, Y, Z, NX, NY, NZ, U, V };
        inline GLfloat cAttrib(unsigned int vertexId, int attribOffset) const {
            return verts[attCmp * vertexId + attribOffset];
        }
        inline GLfloat cAttrib(unsigned int vertexId,
            Attribute attribute) const {
            return cAttrib(vertexId, attToOff(attribute));
        }

        // Utility methods
        // friend class MeshStatistics;     // maybe in the future?
        float avgEdgeLength() const;
        float getVolume() const;

        // Mesh processing
        // friend class MeshProcessing;     // maybe in the future?
        void gaussNoise(float variance, 
            bool normal = true, bool tangential = true);
        void makeCentered();

        class FileOpenException;
        class NotFinalizedException;
        class NoAttributeException;

    protected:
        // Access methods
        inline GLfloat& attrib(unsigned int vertexId, int attribOffset) {
            return verts[attCmp * vertexId + attribOffset];
        }
        inline GLfloat& attrib(unsigned int vertexId, Attribute attribute) {
            return attrib(vertexId, attToOff(attribute));
        }

    private:
        bool final = false;
        bool allocatedGLBuffers = false; // prevent deletion of unalloc. buffers

        unsigned int vNum=0, fNum=0;    // vertex and face count
        vArray verts;
        fArray faces;

        GLuint vbo, ebo, vao;   // Buffer indices
        const GLuint attCnt; // Attribute count
        const GLuint attCmp; // Total number of components
        const size_t attByt; // Total byte offset

        // Converts ttribute enum value to offset
        unsigned int attToOff(Attribute att) const;

        // Vertex-vertex adjacency list
        // typedef std::vector<std::set<unsigned int>> AdjacencyList;

        const bool hasNrm;  // Has normals?
        const bool hasPar;  // Has parametric coordinates?
        
        // Computes normals unless already available
        bool normalsComputed = false;
        void requireNormals(bool recompute = false);
        
};

// Exceptions
class Mesh::FileOpenException : public std::exception {
    public: const char* what() { return "Could not open file"; }
};

class Mesh::NotFinalizedException : public std::exception {
    public: const char* what() { return "Mesh has not been finalised"; }
};

class Mesh::NoAttributeException : public std::exception {
    public: const char* what() { return "Mesh has not been finalised"; }
};

#endif