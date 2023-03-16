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

void PlaneSampling::print(std::string path) {
    // Open file
    std::ofstream file(path);

    // Header
    file << "OFF " << vertNum() << " " << faceNum() << " " << 0 << std::endl;

    // Write verts
    for (uint i=0; i < vertNum(); ++i) {
        file << std::setprecision(DPRECIS)
            << cAttrib(i, Mesh::Attribute::X) << " "
            << std::setprecision(DPRECIS)
            << cAttrib(i, Mesh::Attribute::Y) << " "
            << 0 << std::endl;
    }
    // Write faces
    for (uint i=0; i < faceNum(); ++i) {
        const uint fac[3] = {
            cFacei(i, 0),
            cFacei(i, 1),
            cFacei(i, 2)
        };
        file << "3 "
            << cFacei(i,0) << " "
            << cFacei(i,1) << " "
            << cFacei(i,2) << std::endl;
    }

    // Close file
    file.close();
}

PlaneSampling::PlaneSampling(std::vector<glm::dvec2> positions) {
    // Plane sampling data structures
    verts.clear();
    faces.clear();

    // Data structures to call Triangle
    // moderate amounts of C ahead!
    triangulateio in, out;
    std::string triangleFlags = "zBPOQ";
    
    in.numberofpoints = positions.size();
    in.numberofpointattributes = 0;
    in.pointmarkerlist = (int*) NULL;
    in.pointlist = (double*) malloc(in.numberofpoints*2*sizeof(double));
    for (uint i = 0; i < in.numberofpoints; ++i) {
        const auto v = positions.at(i);
        in.pointlist[2*i+0] = v.x;
        in.pointlist[2*i+1] = v.y;
    }

    out.pointlist = (REAL*) NULL;
    out.trianglelist = (int*) NULL;

    char * c = (char*)malloc(triangleFlags.size() * sizeof(char));
    triangleFlags.copy(c, triangleFlags.size());
    triangulate(c, &in, &out, NULL);
    free(c);

    // Write triangulation
    size_t nv = out.numberofpoints;
    size_t nf = out.numberoftriangles;
    verts.reserve(nv * 2);
    faces.reserve(nf * 3);
    for (uint i = 0; i < nv; ++i) {
        verts.push_back(out.pointlist[2 * i + 0]);
        verts.push_back(out.pointlist[2 * i + 1]);
    }
    for (uint i = 0; i < nf; ++i) {
        faces.push_back(out.trianglelist[3 * i + 0]);
        faces.push_back(out.trianglelist[3 * i + 1]);
        faces.push_back(out.trianglelist[3 * i + 2]);
    }
    print("triangulatedPlane.off");

    free(in.pointlist);
    free(out.pointlist);
    free(out.trianglelist);
}