#include "PlaneSampling.hpp"

PlaneSampling::PlaneSampling(std::string path) {
    verts.clear();
    faces.clear();
	verts.reserve(32);
	faces.reserve(64);
    std::ifstream file(path);
    if (file.is_open()) {
        unsigned int cnt = 0;

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