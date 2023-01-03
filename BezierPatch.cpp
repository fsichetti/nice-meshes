#include "BezierPatch.hpp"

unsigned int BezierPatch::binomial(int k, int n) {
    if (k < 0 || k > n) throw std::domain_error("K must be between 0 and N.");
    if (k == 0 || k == n) return 1;
    const unsigned int nn = n-2;
    const unsigned int ind = (nn*nn + nn)/2 + k-1;
    if (bc[ind] == 0) bc[ind] = binomial(k-1, n-1) + binomial(k, n-1);
    return bc[ind];
}


glm::vec3 BezierPatch::samplePosition(const ControlGrid& cg,
    double u, double v, unsigned int derivU, unsigned int derivV) {
    glm::vec3 p(0);
    for (unsigned int i = 0; i <= degree; ++i) {
        for (int j = 0; j <= degree; ++j) {
            p += glm::vec1(
                bPoly(i, degree, u, derivU) * 
                bPoly(j, degree, v, derivV)
            ) * cg.get(i, j);
        }
    }
    return p;
}


BezierPatch::BezierPatch(ControlGrid cg, unsigned int samples)
    : Mesh(true, true, true) {
    name = "BezierPatch";
    const unsigned int uvSamples = samples * samples;
    const double uvStep = 1.0 / (double)samples;
    reserveSpace(uvSamples, uvSamples/2);

    // Compute points
    // Iterate on sample points (u,v)
    for (unsigned int u = 0; u < samples; ++u) {
        for (unsigned int v = 0; v < samples; ++v) {
            const double uu = u * uvStep, vv = v * uvStep;
            // Iterate on control points (i,j)
            const glm::vec3 x = samplePosition(cg, uu, vv);
            const unsigned int index = addVertex(x[0], x[1], x[2]);

            // Compute normals analitically
            const glm::vec3 xu = samplePosition(cg, uu, vv, 1, 0);
            const glm::vec3 xv = samplePosition(cg, uu, vv, 0, 1);
            const glm::vec3 xuu = samplePosition(cg, uu, vv, 2, 0);
            const glm::vec3 xuv = samplePosition(cg, uu, vv, 1, 1);
            const glm::vec3 xvv = samplePosition(cg, uu, vv, 0, 2);
            const DifferentialQuantities dq(xu, xv, xuu, xuv, xvv);

            // Normals
            attrib(index, Attribute::NX) = dq.normal().x;
            attrib(index, Attribute::NY) = dq.normal().y;
            attrib(index, Attribute::NZ) = dq.normal().z;

            // Parametric coordinates
            attrib(index, Attribute::U) = uu;
            attrib(index, Attribute::V) = vv;
            
            // Curvature
            attrib(index, Attribute::H) = dq.meanCurvature();
            attrib(index, Attribute::K) = dq.gaussianCurvature();

            if (u < samples-1 && v < samples-1) {
                const unsigned int id = samples * u + v;
                addFace(id, id+1, id+samples);
                addFace(id+1, id+samples+1, id+samples);
            }
        }
    }
    computeNormals(true);
}


BezierPatch::BezierPatch(ControlGrid cg, PlaneSampling smp)
    : Mesh(true, true, true) {
    name = "BezierPatch";
    const unsigned int NV = smp.verts.size() / 2;
    const unsigned int NF = smp.faces.size() / 3;
    reserveSpace(NV, NF);

    // Compute vertices
    for (unsigned int i = 0; i < NV; ++i) {
        const double uu = smp.verts[2*i], vv = smp.verts[2*i+1];
        const glm::vec3 x = samplePosition(cg, uu, vv);
        addVertex(x[0], x[1], x[2]);

        // Compute normals analitically
        const glm::vec3 xu = samplePosition(cg, uu, vv, 1, 0);
        const glm::vec3 xv = samplePosition(cg, uu, vv, 0, 1);
        const glm::vec3 xuu = samplePosition(cg, uu, vv, 2, 0);
        const glm::vec3 xuv = samplePosition(cg, uu, vv, 1, 1);
        const glm::vec3 xvv = samplePosition(cg, uu, vv, 0, 2);
        const DifferentialQuantities dq(xu, xv, xuu, xuv, xvv);

        // Normals
        attrib(i, Attribute::NX) = dq.normal().x;
        attrib(i, Attribute::NY) = dq.normal().y;
        attrib(i, Attribute::NZ) = dq.normal().z;

        // Parametric coordinates
        attrib(i, Attribute::U) = uu;
        attrib(i, Attribute::V) = vv;

        // Curvature
        attrib(i, Attribute::H) = dq.meanCurvature();
        attrib(i, Attribute::K) = dq.gaussianCurvature();
    }
    // Add faces
    for (unsigned int i = 0; i < NF; ++i) {
        const auto fi = 3*i;
        addFace(smp.faces[fi], smp.faces[fi+1], smp.faces[fi+2]);
    }
    computeNormals(true);
}


BezierPatch::ControlGrid::ControlGrid(double maxNorm, double bb, double ib) {
    const double radBrd = bb/6.0, radInn = (ib > 0) ? ib/6.0 : bb;
    // Generate patch corners
    glm::vec3 origin(0);
    const auto p0 = RandPoint::onSphere(maxNorm, origin);
    const auto p1 = RandPoint::onSphere(maxNorm, origin);
    const auto p2 = RandPoint::onSphere(maxNorm, origin);
    const auto p3 = RandPoint::onSphere(maxNorm, origin);
    set(0, 0, p0);
    set(3, 0, p1);
    set(3, 3, p2);
    set(0, 3, p3);

    // Generate patch border
    const auto N = glm::vec1(1.0/3.0);  // near weight
    const auto F = glm::vec1(1)-N;  // far weight
    const auto p4 = RandPoint::inSphere((p1-p0).length()*radBrd, N*p0 + F*p1);
    const auto p5 = RandPoint::inSphere((p1-p0).length()*radBrd, F*p0 + N*p1);
    const auto p6 = RandPoint::inSphere((p2-p1).length()*radBrd, N*p1 + F*p2);
    const auto p7 = RandPoint::inSphere((p2-p1).length()*radBrd, F*p1 + N*p2);
    const auto p8 = RandPoint::inSphere((p3-p2).length()*radBrd, N*p2 + F*p3);
    const auto p9 = RandPoint::inSphere((p3-p2).length()*radBrd, F*p2 + N*p3);
    const auto p10 = RandPoint::inSphere((p0-p3).length()*radBrd, N*p3 + F*p0);
    const auto p11 = RandPoint::inSphere((p0-p3).length()*radBrd, F*p3 + N*p0);
    set(1, 0, p4);
    set(2, 0, p5);
    set(3, 1, p6);
    set(3, 2, p7);
    set(2, 3, p8);
    set(1, 3, p9);
    set(0, 2, p10);
    set(0, 1, p11);

    // Generate patch interior
    const auto N2 = glm::vec1(.5)*N;
    const auto F2 = glm::vec1(.5)*F;
    const auto p12 = RandPoint::inSphere(
        glm::min((p4-p9).length(), (p6-p11).length())*radInn, 
        N2*(p4+p11)+F2*(p6+p9)
    );
    const auto p13 = RandPoint::inSphere(
        glm::min((p5-p8).length(), (p6-p11).length())*radInn,
        N2*(p5+p6)+F2*(p8+p11)
    );
    const auto p14 = RandPoint::inSphere(
        glm::min((p5-p8).length(), (p7-p10).length())*radInn,
        N2*(p5+p10)+F2*(p8+p7)
    );
    const auto p15 = RandPoint::inSphere(
        glm::min((p4-p9).length(), (p7-p10).length())*radInn,
        N2*(p9+p10)+F2*(p4+p7)
    );
    set(1,1,p12);
    set(2,1,p13);
    set(2,2,p14);
    set(1,2,p15);
}


void BezierPatch::PlaneSampling::readFromObj(std::string path) {
    verts.clear();
    faces.clear();
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
    else throw FileOpenException();
    file.close();
}