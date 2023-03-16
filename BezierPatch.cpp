#include "BezierPatch.hpp"

void BezierPatch::bcPrepare() {
    uint k = ((degree-1) * (degree-1) + degree-1) / 2;
    bc = new uint[k];
    for (uint i = 0; i < k; ++i) {
        bc[i] = 0;
    }
}

uint BezierPatch::binomial(int k, int n) const {
    if (k < 0 || k > n) throw std::domain_error("K must be between 0 and N.");
    if (k == 0 || k == n) return 1;
    const uint nn = n-2;
    const uint ind = (nn*nn + nn)/2 + k-1;
    if (bc[ind] == 0) bc[ind] = binomial(k-1, n-1) + binomial(k, n-1);
    return bc[ind];
}


glm::dvec3 BezierPatch::sampleSurface(
    double u, double v, uint derivU, uint derivV) const {
    glm::dvec3 p(0);
    for (uint i = 0; i <= degree; ++i) {
        for (int j = 0; j <= degree; ++j) {
            p += glm::dvec1(
                bPoly(i, degree, u, derivU) * 
                bPoly(j, degree, v, derivV)
            ) * control->get(i, j);
        }
    }
    return p;
}


BezierPatch::BezierPatch(const ControlGrid *const cg, uint samples)
    : Mesh(true, true, true), control(cg) {
    name = "BezierPatch";
    const uint uvSamples = samples * samples;
    const double uvStep = 1.0 / static_cast<double>(samples-1);
    reserveSpace(uvSamples, uvSamples/2);
    bcPrepare();

    // Compute points
    // Iterate on sample points (u,v)
    for (uint u = 0; u < samples; ++u) {
        for (uint v = 0; v < samples; ++v) {
            const double uu = u * uvStep, vv = v * uvStep;
            // Iterate on control points (i,j)
            const glm::dvec3 x = sampleSurface(uu, vv);
            const uint index = addVertex(x[0], x[1], x[2]);

            // Compute normals analitically
            const glm::dvec3 xu = sampleSurface(uu, vv, 1, 0);
            const glm::dvec3 xv = sampleSurface(uu, vv, 0, 1);
            const glm::dvec3 xuu = sampleSurface(uu, vv, 2, 0);
            const glm::dvec3 xuv = sampleSurface(uu, vv, 1, 1);
            const glm::dvec3 xvv = sampleSurface(uu, vv, 0, 2);
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
                const uint id = samples * u + v;
                addFace(id, id+1, id+samples);
                addFace(id+1, id+samples+1, id+samples);
            }
        }
    }
    computeNormals(true);
}


BezierPatch::BezierPatch(const ControlGrid *const cg, PlaneSampling smp)
    : Mesh(true, true, true), control(cg) {
    name = "BezierPatch";
    const uint NV = smp.vertNum();
    const uint NF = smp.faceNum();
    reserveSpace(NV, NF);
    bcPrepare();

    // Compute vertices
    for (uint i = 0; i < NV; ++i) {
        const double uu = smp.cAttrib(i, 0), vv = smp.cAttrib(i, 1);
        const glm::dvec3 x = sampleSurface(uu, vv);
        addVertex(x[0], x[1], x[2]);

        // Compute normals analitically
        const DifferentialQuantities dq = diffEvaluate(uu, vv);

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
    for (uint i = 0; i < NF; ++i) {
        addFace(smp.cFacei(i, 0), smp.cFacei(i, 1), smp.cFacei(i, 2));
    }
    computeNormals(true);
}

BezierPatch::BezierPatch(const ControlGrid *const cg, uint samples, double aniso)
    /*
    This monstrosity creates a simple "base" uniform mesh, samples it,
    triangulates it, and finally calls the PlaneSampling constructor.
    Anisotropy is currently ignored.
    */
    : BezierPatch(cg, 
        PlaneSampling(
            BezierPatch(cg,16).uniformSampling(
                samples, true, 4*std::floor(sqrt(samples)))
        )
    ) {}

BezierPatch::~BezierPatch() {
    delete control;
    delete[] bc;
}



BezierPatch::ControlGrid::ControlGrid(double maxNorm, double bb, double ib) {
    const double radBrd = bb/6.0, radInn = (ib > 0) ? ib/6.0 : bb;
    // Generate patch corners
    glm::dvec3 origin(0);
    const auto p0 = RandPoint::onSphere(maxNorm) + origin;
    const auto p1 = RandPoint::onSphere(maxNorm) + origin;
    const auto p2 = RandPoint::onSphere(maxNorm) + origin;
    const auto p3 = RandPoint::onSphere(maxNorm) + origin;
    set(0, 0, p0);
    set(3, 0, p1);
    set(3, 3, p2);
    set(0, 3, p3);

    // Generate patch border
    const auto N = glm::dvec1(1.0/3.0);  // near weight
    const auto F = glm::dvec1(1)-N;  // far weight
    const auto p4 = RandPoint::inSphere((p1-p0).length()*radBrd) + N*p0 + F*p1;
    const auto p5 = RandPoint::inSphere((p1-p0).length()*radBrd) + F*p0 + N*p1;
    const auto p6 = RandPoint::inSphere((p2-p1).length()*radBrd) + N*p1 + F*p2;
    const auto p7 = RandPoint::inSphere((p2-p1).length()*radBrd) + F*p1 + N*p2;
    const auto p8 = RandPoint::inSphere((p3-p2).length()*radBrd) + N*p2 + F*p3;
    const auto p9 = RandPoint::inSphere((p3-p2).length()*radBrd) + F*p2 + N*p3;
    const auto p10 = RandPoint::inSphere((p0-p3).length()*radBrd) + N*p3 + F*p0;
    const auto p11 = RandPoint::inSphere((p0-p3).length()*radBrd) + F*p3 + N*p0;
    set(1, 0, p4);
    set(2, 0, p5);
    set(3, 1, p6);
    set(3, 2, p7);
    set(2, 3, p8);
    set(1, 3, p9);
    set(0, 2, p10);
    set(0, 1, p11);

    // Generate patch interior
    const auto N2 = glm::dvec1(.5)*N;
    const auto F2 = glm::dvec1(.5)*F;
    const auto p12 = RandPoint::inSphere(
        glm::min((p4-p9).length(), (p6-p11).length())*radInn) +
        N2*(p4+p11)+F2*(p6+p9);
    const auto p13 = RandPoint::inSphere(
        glm::min((p5-p8).length(), (p6-p11).length())*radInn) +
        N2*(p5+p6)+F2*(p8+p11);
    const auto p14 = RandPoint::inSphere(
        glm::min((p5-p8).length(), (p7-p10).length())*radInn) +
        N2*(p5+p10)+F2*(p8+p7);
    const auto p15 = RandPoint::inSphere(
        glm::min((p4-p9).length(), (p7-p10).length())*radInn) +
        N2*(p9+p10)+F2*(p4+p7);
    set(1,1,p12);
    set(2,1,p13);
    set(2,2,p14);
    set(1,2,p15);
}

DifferentialQuantities BezierPatch::diffEvaluate(double u, double v) const {
    const glm::dvec3 xu = sampleSurface(u, v, 1, 0);
    const glm::dvec3 xv = sampleSurface(u, v, 0, 1);
    const glm::dvec3 xuu = sampleSurface(u, v, 2, 0);
    const glm::dvec3 xuv = sampleSurface(u, v, 1, 1);
    const glm::dvec3 xvv = sampleSurface(u, v, 0, 2);
    return DifferentialQuantities(xu, xv, xuu, xuv, xvv);
}


void BezierPatch::ControlGrid::writeCoordinate(
    std::string path, int coordinate) {
    // Open file
    std::ofstream file(path);
    if (!file.is_open()) throw Mesh::FileOpenException();

    // Write verts
    for (uint i = 0; i <= degree; ++i) {
        for (uint j = 0; j <= degree; ++j) {
            file << std::setprecision(DPRECIS) << get(i,j)[coordinate] << " ";
        }
        file << std::endl;
    }

    // Close file
    file.close();
}