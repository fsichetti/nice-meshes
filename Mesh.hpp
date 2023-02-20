#ifndef MESH_H
#define MESH_H

#include <unordered_map>
#include <vector>
#include <fstream>
#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include "RandPoint.hpp"

#include "DifferentialQuantities.hpp"

class Mesh { 
    public:
        typedef std::vector<GLfloat> vArray;
        typedef std::vector<GLuint> fArray;

        std::string name = "";

        // Init mesh with expected number of vertices and faces
        Mesh(bool normals, bool parametric, bool curvature);
        void deleteBuffers() {
            if (!allocatedGLBuffers) return;
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
            vbo = 0;
            ebo = 0;
            vao = 0;
            allocatedGLBuffers = false;
        }

        // Get vertex and face number
        const inline unsigned int vertNum() const { return vNum; }
        const inline unsigned int faceNum() const { return fNum; }

        // Core methods
        void reserveSpace(unsigned int verts, unsigned int faces);
        unsigned int addVertex();
        unsigned int addVertex(GLfloat x, GLfloat y, GLfloat z);
        unsigned int addFace(GLuint i, GLuint j, GLuint k);
        void finalize(bool nogui = false);
        void draw(GLuint drawMode = GL_TRIANGLES) const;

        // File input
        void readOBJ(std::string filename);

        // File output
        void writePLY(std::string filename) const;
        void writeOBJ(std::string filename) const;
        void writeOFF(std::string filename) const;

        // Access methods
        enum Attribute { X, Y, Z, NX, NY, NZ, U, V, K, H };
        const inline GLfloat cAttrib(unsigned int vertexId,
            unsigned int attribOffset) const {
            return verts[attCmp * vertexId + attribOffset];
        }
        const inline GLfloat cAttrib(unsigned int vertexId,
            Attribute attribute) const {
            return cAttrib(vertexId, attToOff(attribute));
        }
        const inline GLuint cFacei(unsigned int faceId, unsigned int n) const {
            return faces[3 * faceId + n];
        }

        // Utility methods
        // friend class MeshStatistics;     // maybe in the future?
        float avgEdgeLength() const;
        float getVolume() const;

        // Differential quantities
        virtual DifferentialQuantities diffEvaluate(double u, double v)
            const;
        inline glm::vec3 gradient(double u, double v, double f,
            double fu, double fv) const {
            return diffEvaluate(u, v).gradient(f, fu, fv);
        }
        inline double laplacian(double u, double v, double f,
            double fu, double fv, double fuu, double fuv, double fvv) const {
            return diffEvaluate(u, v).laplacian(f, fu, fv, fuu, fuv, fvv);
        }
        inline glm::vec3 hessian(double u, double v, double f,
            double fu, double fv, double fuu, double fuv, double fvv) const {
            return diffEvaluate(u, v).hessian(f, fu, fv, fuu, fuv, fvv);
        }

        // Mesh processing
        // friend class MeshProcessing;     // maybe in the future?
        void gaussNoise(float variance, 
            bool normal = true, bool tangential = true);
        void makeCentered();
        void refine();

        class FileOpenException;
        class NotFinalizedException;
        class NoAttributeException;

    protected:
        // Access methods
        inline GLfloat& attrib(unsigned int vertexId, unsigned int attribOffset) {
            return verts[attCmp * vertexId + attribOffset];
        }
        inline GLfloat& attrib(unsigned int vertexId, Attribute attribute) {
            return attrib(vertexId, attToOff(attribute));
        }
        inline GLuint& facei(unsigned int faceId, unsigned int n) {
            return faces[3 * faceId + n];
        }

        // Normals computation
        void computeNormals(bool noCompute = false /*only mark as computed*/);

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

        // Converts attribute enum value to offset
        unsigned int attToOff(Attribute att) const;

        // Vertex-vertex adjacency list
        // typedef std::vector<std::set<unsigned int>> adjacencyVV;

        const bool hasNrm;  // Has normals?
        const bool hasPar;  // Has parametric coordinates?
        const bool hasDif;  // Has curvature / other differential quantities?
        
        // Computes normals unless already available
        bool normalsComputed = false;
};

// Exceptions
class Mesh::FileOpenException : public std::exception {
    public: const char* what() { return "Could not open file"; }
};

class Mesh::NotFinalizedException : public std::exception {
    public: const char* what() { return "Mesh has not been finalised"; }
};

class Mesh::NoAttributeException : public std::exception {
    public: const char* what() { return "Mesh is missing an attribute"; }
};

#endif