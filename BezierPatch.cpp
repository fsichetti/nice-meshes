#include "BezierPatch.hpp"

BezierPatch::BezierPatch(ControlGrid cg, unsigned int samples)
    : Mesh(true, true) {
    name = "RegularTorus";
    const unsigned int uvSamples = samples * samples;
    reserveSpace(uvSamples, uvSamples/2);
}