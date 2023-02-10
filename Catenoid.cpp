#include "Catenoid.hpp"

Catenoid::Catenoid(
        unsigned int samples,   // samples in toroidal direction
        double rOuter,
        double rInner
    ) : Mesh(true, true, true), rOuter(rOuter), rInner(rInner),
        height(2 * rInner * acosh(rOuter / rInner)) {
    assert(rOuter > rInner);

    // Sampling in rotational direction (uniform)
    const unsigned int uSamples = samples;
    const double uStep = 1.0 / static_cast<double>(uSamples);
    const double halfHeight = height / 2;

    // Pre-computation for sampling in vertical direction
    const double temp = TWOPI * SQRT3_2 * uStep * rInner;
    const unsigned int vSamples = std::ceil(height / temp);
    const double vStep = 1.0 / static_cast<double>(vSamples-1);
    // rescale to [0, 1]
    const unsigned int uvSamples = uSamples * vSamples;

    // Create object
    name = "RegularCatenoid";
    reserveSpace(uvSamples, 2*uvSamples);        


    for (unsigned int v = 0; v < vSamples; ++v) {
        for (unsigned int u = 0; u < uSamples; ++u) {
            // Place vertices
            const double uu = (u - (v%2)*.5) * uStep;
            const double vv = v * vStep;
            const unsigned int index = placeVertex(uu, vv);
            
            // Add faces
            if (v != vSamples-1) {    // Open at the ends
                const unsigned int us = (u+uSamples-v%2)%uSamples;
                GLuint a = index;
                GLuint b = (u+1)%uSamples+v*uSamples;
                GLuint c = ((u+1-v%2)%uSamples+(v+1)*uSamples)%uvSamples;
                GLuint d = ((us+uSamples)%uSamples+(v+1)*uSamples)%uvSamples;

                addFace(a, b, c);
                addFace(a, c, d);
            }
        }
    }
    computeNormals(true);
}


Catenoid::Catenoid(std::string path, double rOuter, double rInner) :
    Mesh(true, true, true), rInner(rInner), rOuter(rOuter), 
    height(2 * rInner * acosh(rOuter / rInner)) {
    assert(rOuter > rInner);

    // Read plane parameterization
    PlaneSampling plane(path);
    const unsigned int pv = plane.vertNum(), pf = plane.faceNum();
    reserveSpace(pv, pf);

    // Place vertices
    std::vector<unsigned long> boundary;   // boundary vertices on plane
    std::vector<unsigned long> newId;   // vertex index on the surface
    
    for (unsigned long int i = 0; i < pv; ++i) {
        const double u = plane.cAttrib(i, 0);
        const double v = plane.cAttrib(i, 1);
        // Boundary vertex duplicate checking
        if (u == 0. || u == 1.) {
            bool duplicate = false;
            for (unsigned long j : boundary) {
                const double uj = plane.cAttrib(j, 0);
                const double vj = plane.cAttrib(j, 1);
                if ((uj == 0. || uj == 1.) && v == vj) {
                    duplicate = true;
                    newId.push_back(newId.at(j));
                    break;
                }
            }
            if (!duplicate) {
                boundary.push_back(i);
                newId.push_back(placeVertex(u, v));
            }
        }
        else {
            newId.push_back(placeVertex(u, v));
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


unsigned int Catenoid::placeVertex(double u, double v) {
    const double vs = (v-.5)*height;
    const double sinu = sin(TWOPI * u);
    const double cosu = cos(TWOPI * u);
    const double sinhv = sinh(vs / rInner);
    const double coshv = cosh(vs / rInner);

    const unsigned int index = addVertex();

    attrib(index, Attribute::X) = rInner * coshv * cosu;
    attrib(index, Attribute::Y) = rInner * coshv * sinu;
    attrib(index, Attribute::Z) = vs;

    // Normals
    const double nrmFac = 1/coshv;
    attrib(index, Attribute::NX) = cosu * nrmFac;
    attrib(index, Attribute::NY) = sinu * nrmFac;
    attrib(index, Attribute::NZ) = -sinhv * nrmFac;

    // Write parametric coordinates
    attrib(index, Attribute::U) = u;
    attrib(index, Attribute::V) = v;

    // Curvature
    attrib(index, Attribute::H) = 0;
    attrib(index, Attribute::K) = -pow(rInner, -2) * pow(coshv, -4);

    return index;
}