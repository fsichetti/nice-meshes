#include "Torus.hpp"

Torus::Torus(
        uint samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true, true), rInner(rInner), rOuter(rOuter) {

    // Construct torus
    const double rRatio = rOuter / rInner;
    const uint uSamples = samples;
    const double uStep = 1.0 / static_cast<double>(uSamples);

    // Pre-computation for sampling in poloidal direction
    double phiMax = 0;
    uint count = 0;
    const double vStep = SQRT3_2 * uStep;
    // rescale to [0, 1]
    while (phiMax < TWOPI || count % 2) { 
        phiMax += TWOPI * vStep * (rRatio + cos(phiMax));
        ++count;
    }
    const uint vSamples = count;
    const uint uvSamples = uSamples * vSamples;

    name = "RegularTorus";
    reserveSpace(uvSamples, 2*uvSamples);    


    double phi = 0;
    for (uint v = 0; v < vSamples; ++v) {
        for (uint u = 0; u < uSamples; ++u) {
            // Place vertices
            double uu = (u - (v%2)*.5) * uStep;
            double vv = phi / phiMax;
            if (uu < 0) uu += 1.;
            assert(uu >= 0 && vv >= 0 && uu <= 1 && vv <= 1);
            
            const uint index = placeVertex(uu, vv);

            // Add faces
            const uint us = (u+uSamples-v%2)%uSamples;
            uint a = index;
            uint b = (u+1)%uSamples+v*uSamples;
            uint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
            uint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

            addFace(a, b, c);
            addFace(a, c, d);
        }
        phi += TWOPI * vStep * (rRatio + cos(phi));
    }
    computeNormals(true);
}


Torus::Torus(
        std::string path,
        double rOuter,
        double rInner
    ) : Mesh(true, true, true), rInner(rInner), rOuter(rOuter) {

    // Construct torus
    readOBJ(path);
    // Projection
    const uint s = vertNum();
    for (uint i = 0; i < s; ++i) {
        const double x = cAttrib(i, Attribute::X);
        const double y = cAttrib(i, Attribute::Y);
        const double z = cAttrib(i, Attribute::Z);
        // Project on torus of given radius
        // First determine toroidal and poloidal angles, then project
        const double nxy = 1.0 / sqrt(x*x + y*y);
        const double theta = (y*nxy>=0) ? acos(x*nxy) : TWOPI - acos(x*nxy);
        const double w = cos(theta) * x + sin(theta) * y - rOuter;
        const double nwz = 1.0 / sqrt(w*w + z*z);
        const double phi = (z*nwz>=0) ? acos(w*nwz) : TWOPI - acos(w*nwz);
        replaceVertex(i, theta / TWOPI, phi / TWOPI);
    }
    computeNormals(true);
}


Torus::Torus(const PlaneSampling& plane, double rOuter, double rInner) :
    Mesh(true, true, true), rInner(rInner), rOuter(rOuter) {
    
    assert(rOuter > rInner);

    // Read plane parameterization
    const uint pv = plane.vertNum(), pf = plane.faceNum();
    reserveSpace(pv, pf);

    // Place vertices
    std::vector<unsigned long> boundary;   // boundary vertices on plane
    std::vector<unsigned long> newId;   // vertex index on the surface
    newId.reserve(pv);
    
    for (unsigned long int i = 0; i < pv; ++i) {
        const double ui = plane.cAttrib(i, 0), vi = plane.cAttrib(i, 1);
        const double mui = ui - floor(ui), mvi = vi - floor(vi);
        // Boundary vertex duplicate checking
        if (mui == 0. || mvi == 0.) {
            bool duplicate = false;
            for (unsigned long j : boundary) {
                const double uj = plane.cAttrib(j, 0), vj = plane.cAttrib(j, 1);
                const double muj = uj - floor(uj), mvj = vj - floor(vj);
                if (mui == muj && mvi == mvj) {
                    duplicate = true;
                    newId.push_back(newId.at(j));
                    break;
                }
            }
            if (!duplicate) {
                boundary.push_back(i);
                newId.push_back(placeVertex(ui, vi));
            }
        }
        else {
            newId.push_back(placeVertex(ui, vi));
        }
    }

    // Write faces w/ substitutions
    for (uint i = 0; i < pf; ++i) {
        addFace(
            newId.at(plane.cFacei(i, 0)),
            newId.at(plane.cFacei(i, 1)),
            newId.at(plane.cFacei(i, 2))
        );
    }
    computeNormals(true);
}


Torus::Torus(uint samples, double rOuter, double rInner, double aniso) :
    /*
    This monstrosity creates a simple "base" uniform mesh, samples it,
    triangulates it, and finally calls the PlaneSampling constructor.
    Anisotropy is currently ignored.
    */
    Torus(PlaneSampling(
        Torus(16, rOuter, rInner).uniformSampling(
            samples, true, 4 * std::floor(sqrt(samples)))
        ), rOuter, rInner
    ) {}


void Torus::replaceVertex(uint index, double u, double v) {
    const double sinu = sin(TWOPI * u);
    const double cosu = cos(TWOPI * u);
    const double sinv = sin(TWOPI * v);
    const double cosv = cos(TWOPI * v);
    
    attrib(index, Attribute::X) = cosu * (rOuter + rInner * cosv);
    attrib(index, Attribute::Y) = sinu * (rOuter + rInner * cosv);
    attrib(index, Attribute::Z) = rInner * sinv;

    // Normals
    attrib(index, Attribute::NX) = cosu * cosv;
    attrib(index, Attribute::NY) = sinu * cosv;
    attrib(index, Attribute::NZ) = sinv;

    // Parametric coordinates
    attrib(index, Attribute::U) = u;
    attrib(index, Attribute::V) = v;

    // Curvature
    attrib(index, Attribute::K) = cosv /
        (rInner * (rOuter + rInner * cosv));
    attrib(index, Attribute::H) = (rOuter + 2 * rInner * cosv) /
        (2 * rInner * (rOuter + rInner * cosv));
}

uint Torus::placeVertex(double u, double v) {
    const uint index = addVertex();
    replaceVertex(index, u, v);
    return index;
}


DifferentialQuantities Torus::diffEvaluate(double u, double v) const {
    const double sinu = sin(TWOPI * u);
    const double cosu = cos(TWOPI * u);
    const double sinv = sin(TWOPI * v);
    const double cosv = cos(TWOPI * v);
    glm::dvec3 xu(
        -sinu * (rOuter + rInner * cosv) * TWOPI,
        cosu * (rOuter + rInner * cosv) * TWOPI,
        0
    );
    glm::dvec3 xv(
        -rInner * cosu * sinv * TWOPI,
        -rInner * sinu * sinv * TWOPI,
        rInner * cosv * TWOPI
    );
    glm::dvec3 xuu(
        -cosu * (rOuter + rInner * cosv) * pow(TWOPI, 2),
        -sinu * (rOuter + rInner * cosv) * pow(TWOPI, 2),
        0
    );
    glm::dvec3 xuv(
        -sinu * (-rInner * sinv * TWOPI) * TWOPI,
        cosu * (-rInner * sinv * TWOPI) * TWOPI,
        0
    );
    glm::dvec3 xvv(
        -rInner * cosu * cosv * pow(TWOPI, 2),
        -rInner * sinu * cosv * pow(TWOPI, 2),
        -rInner * sinv * TWOPI * TWOPI
    );
    return DifferentialQuantities(xu, xv, xuu, xuv, xvv);
}


// double Torus::laplacian(double u, double v, double f,
//     double fu, double fv, double fuu, double fuv, double fvv) const {
//     const double k = rOuter + rInner * cos(TWOPI * v);
//     return (sin(TWOPI * v) * fv) / (rInner * k) - fuu / pow(k,2)
//         - fvv / pow(rInner,2);
// }