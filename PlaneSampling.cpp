#include "PlaneSampling.hpp"

PlaneSampling::PlaneSampling(std::string path) {
    verts.clear();
    faces.clear();
	verts.reserve(32);
	faces.reserve(64);
    std::ifstream file(path);
    if (file.is_open()) {
        uint cnt = 0;

        while (!file.eof()) {
            std::string token;
            file >> token;
            if (token.empty()) continue;

            char c0 = token[0];
            bool vfFlag;  // reading vertex/face
            switch (c0) {
                case 'v':
                    vfFlag = true;
                    cnt = 0;
                    break;
                case 'f':
                    vfFlag = false;
                    cnt = 0;
                    break;
                default:
                    if (vfFlag && cnt<2) {
                        verts.push_back(std::stof(token));
                    }
                    else if (!vfFlag && cnt<3) {
                        faces.push_back(std::stoi(token)-1);
                    }
                    ++cnt;
            }
        }
    }
    else throw Mesh::FileOpenException();
    file.close();
}

PlaneSampling::PlaneSampling(std::vector<glm::dvec2> positions) {
    // Plane sampling data structures
    verts.clear();
    faces.clear();
    size_t nv = positions.size();
    verts.reserve(nv*2);

    // Data structures to call Triangle
    // moderate amounts of C ahead!
    triangulateio in, out;
    std::string triangleFlags = "zYYPN";
    
    in.numberofpoints = nv;
    in.numberofpointattributes = 0;
    in.pointmarkerlist = (int*) NULL;
    in.pointlist = (REAL*) malloc(nv*2*sizeof(REAL));
    for (uint i = 0; i < nv; ++i) {
        const auto v = positions.at(i);
        verts.push_back(v.x);
        verts.push_back(v.y);
        in.pointlist[2*i+0] = v.x;
        in.pointlist[2*i+1] = v.y;
    }
    
    out.trianglelist = (int*) NULL;

    char * c = (char*)malloc(triangleFlags.size() * sizeof(char));
    triangleFlags.copy(c, triangleFlags.size());
    triangulate(c, &in, &out, NULL);

    // Write triangulation
    size_t nf = out.numberoftriangles;
    faces.reserve(nf * 3);
    for (uint i = 0; i < nf; ++i) {
        faces.push_back(out.trianglelist[3 * i + 0]);
        faces.push_back(out.trianglelist[3 * i + 1]);
        faces.push_back(out.trianglelist[3 * i + 2]);
    }
}