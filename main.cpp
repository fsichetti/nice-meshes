#include "Browser.hpp"
#include "Torus.hpp"
#include "Catenoid.hpp"
#include "BezierPatch.hpp"

int main(int argc, char **argv) {
    Trackball trb;
    // Create GUI
    Browser::init(argv[0], &trb);

    // Create mesh
    // Mesh* mesh = new Catenoid(100, 2, 1);
    // Mesh* mesh = new Torus(100, 2, 1);
    BezierPatch::ControlGrid cg;
    for (int i = 0; i < 16; ++i) {
        cg.x(i) = i % 4;
        cg.y(i) = (int)(i / 4);
        cg.z(i) = 0;
    }
    Mesh* mesh = new BezierPatch(cg, 16);
    
    Browser::setMesh(mesh);
    Browser::launch();
    return 0;
}