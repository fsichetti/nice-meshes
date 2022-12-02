#include "BezierPatch.hpp"

// Compute and cache the binomial coefficients
void BezierPatch::cacheBinomials() {
    const unsigned int facN = 1;
    binomial[0] = 1;
    binomial[degree] = 1;
    for (unsigned int i = 1; i < degree; ++i) {
        unsigned int num = 1, den = 1;
        // degree! / i!
        for (unsigned int j = i; j <= degree; ++j) {
            num *= j;
        }
        for (unsigned int j = 2; j <= i; ++j) {
            den *= j;
        }
        binomial[i] = num / den;
    }
}


BezierPatch::BezierPatch(ControlGrid cg, unsigned int samples)
    : Mesh(true, true) {
    name = "Bezier";
    const unsigned int uvSamples = samples * samples;
    const double uvStep = TWOPI / (double)samples;
    reserveSpace(uvSamples, uvSamples/2);

    // Compute points
    cacheBinomials();
    // Iterate on sample points (u,v)
    for (int u = 0; u < samples; ++u) {
        for (int v = 0; v < samples; ++v) {
            // Iterate on control points (i,j)
            double p[3] = {0,0,0};
            for (int i = 0; i <= degree; ++i) {
                for (int j = 0; j <= degree; ++j) {
                    const double a = bPoly(i, u);
                    const double b = bPoly(j, v);
                    // Iterate on the 3 coordinates
                    for (int x = 0; x < 2; ++x) {
                        p[x] += a * b * cg.at(i,j,x);
                    }
                }
            }
            addVertex(p[0], p[1], p[2]);
            if (u < samples-1 && v < samples-1) {
                const unsigned int id = u + samples * v;
                addFace(id, id+1, id+samples);
                addFace(id+1, id+samples, id+samples+1);
            }
        }
    }
}