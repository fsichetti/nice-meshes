#include "Torus.hpp"

Torus::Torus(
        unsigned int samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true, true), rInner(rInner), rOuter(rOuter) {

    // Construct torus
    const double rRatio = rOuter / rInner;
    const unsigned int uSamples = samples;
    const double uStep = 1.0 / static_cast<double>(uSamples);

    // Pre-computation for sampling in poloidal direction
    double phiMax = 0;
    unsigned int count = 0;
    const double vStep = SQRT3_2 * uStep;
    // rescale to [0, 1]
    while (phiMax < TWOPI || count % 2) { 
        phiMax += TWOPI * vStep * (rRatio + cos(phiMax));
        ++count;
    }
    const unsigned int vSamples = count;
    const unsigned int uvSamples = uSamples * vSamples;

    name = "RegularTorus";
    reserveSpace(uvSamples, 2*uvSamples);    


    double phi = 0;
    for (unsigned int v = 0; v < vSamples; ++v) {
        for (unsigned int u = 0; u < uSamples; ++u) {
            // Place vertices
            const double uu = (u - (v%2)*.5) * uStep;
            const double vv = phi / phiMax;
            
            const unsigned int index = placeVertex(uu, vv);

            // Add faces
            const unsigned int us = (u+uSamples-v%2)%uSamples;
            GLuint a = index;
            GLuint b = (u+1)%uSamples+v*uSamples;
            GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
            GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

            addFace(a, b, c);
            addFace(a, c, d);
        }
        phi += TWOPI * vStep * (rRatio + cos(phi));
    }
    computeNormals(true);
}


Torus::Torus(std::string path, double rOuter, double rInner) :
    Mesh(true, true, true), rInner(rInner), rOuter(rOuter) {
    
    assert(rOuter > rInner);

    // Read plane parameterization
    PlaneSampling plane(path);
    const unsigned int pv = plane.vertNum(), pf = plane.faceNum();
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
    for (unsigned int i = 0; i < pf; ++i) {
        addFace(
            newId.at(plane.cFacei(i, 0)),
            newId.at(plane.cFacei(i, 1)),
            newId.at(plane.cFacei(i, 2))
        );
    }
    computeNormals(true);
}


unsigned int Torus::placeVertex(double u, double v) {
    const double sinu = sin(TWOPI * u);
    const double cosu = cos(TWOPI * u);
    const double sinv = sin(TWOPI * v);
    const double cosv = cos(TWOPI * v);
    
    const unsigned int index = addVertex();
    
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

    return index;
}


double Torus::laplacian(double u, double v, double f,
    double fu, double fv, double fuu, double fuv, double fvv) const {
    const double k = rOuter + rInner * cos(TWOPI * v);
    return (sin(TWOPI * v) * fv) / (rInner * k) - fuu / pow(k,2)
        - fvv / pow(rInner,2);
}