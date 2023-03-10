#ifndef MESH_H
#define MESH_H

#include <unordered_map>
#include <vector>
#include <fstream>
#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include <iomanip>

#include "RandPoint.hpp"
#include "DifferentialQuantities.hpp"

const size_t DPRECIS = std::numeric_limits<double>::digits10 + 1;


class Mesh { 
    public:
        typedef std::vector<double> vArray;
        typedef std::vector<uint> fArray;

        std::string name = "";

        // Init mesh with expected number of vertices and faces
        Mesh(bool normals, bool parametric, bool curvature);
        void deleteBuffers();

        // Get vertex and face number
        const inline uint vertNum() const { return vNum; }
        const inline uint faceNum() const { return fNum; }

        // Core methods
        void reserveSpace(uint verts, uint faces);
        uint addVertex();
        uint addVertex(double x, double y, double z);
        uint addFace(uint i, uint j, uint k);
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
        const inline double cAttrib(uint vertexId,
            uint attribOffset) const {
            return verts[attCmp * vertexId + attribOffset];
        }
        const inline double cAttrib(uint vertexId,
            Attribute attribute) const {
            return cAttrib(vertexId, attToOff(attribute));
        }
        const inline uint cFacei(uint faceId, uint n) const {
            return faces[3 * faceId + n];
        }

        // Utility methods
        // friend class MeshStatistics;     // maybe in the future?
        double avgEdgeLength() const;
        double getVolume() const;

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
        void gaussNoise(double variance, 
            bool normal = true, bool tangential = true);
        void makeCentered();
        void refine();

        class FileOpenException;
        class NotFinalizedException;
        class NoAttributeException;

    protected:
        // Access methods
        inline double& attrib(uint vertexId, uint attribOffset) {
            return verts[attCmp * vertexId + attribOffset];
        }
        inline double& attrib(uint vertexId, Attribute attribute) {
            return attrib(vertexId, attToOff(attribute));
        }
        inline uint& facei(uint faceId, uint n) {
            return faces[3 * faceId + n];
        }

        // Normals computation
        void computeNormals(bool noCompute = false /*only mark as computed*/);

    private:
        bool final = false;
        bool allocatedGLBuffers = false; // prevent deletion of unalloc. buffers

        uint vNum=0, fNum=0;    // vertex and face count
        vArray verts;
        fArray faces;

        GLuint vbo, ebo, vao;   // Buffer indices
        const uint attCnt; // Attribute count
        const uint attCmp; // Total number of components

        // Converts attribute enum value to offset
        uint attToOff(Attribute att) const;

        // Vertex-vertex adjacency list
        // typedef std::vector<std::set<uint>> adjacencyVV;

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