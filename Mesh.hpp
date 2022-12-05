#ifndef MESH_H
#define MESH_H

#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <epoxy/gl.h>
#include <glm/glm.hpp>

class Mesh { 
    public:
        typedef std::vector<GLfloat> vArray;
        typedef std::vector<GLuint> fArray;

        std::string name = "mesh";

        // Init mesh with expected number of vertices and faces
        Mesh(bool normals, bool parametric);
        ~Mesh();

        void reserveSpace(unsigned int verts, unsigned int faces);
        void finalize();
        void draw(GLuint drawMode = GL_TRIANGLES) const;
        void writeToObj(std::string filename) const;
        void writeToPly(std::string filename) const;
        float getVolume() const;

        unsigned int addVertex(GLfloat x, GLfloat y, GLfloat z);
        unsigned int addFace(GLuint i, GLuint j, GLuint k);
        GLfloat* vComponent(int vertex_index, int attrib_offset);

    private:
        bool final = false;

        unsigned int vNum=0, fNum=0;    // vertex and face count
        vArray verts;
        fArray faces;

        // Vertex-vertex adjacency list
        // typedef std::vector<std::set<unsigned int>> AdjacencyList;

        const bool hasNrm;  // Has normals?
        const bool hasPar;  // Has parametric cooridnates?
        
        void computeNormals();

        const GLuint attCnt; // Attribute count
        const GLuint attCmp; // Total number of components
        const size_t attByt; // Total byte offset
            
        GLuint vbo, ebo, vao;
};

class MeshNotFinalizedException : public std::exception {
    public: const char* what() { return "Mesh has not been finalised"; }
};

#endif