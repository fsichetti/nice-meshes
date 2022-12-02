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
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cg.at(i,j,0) = i;
            cg.at(i,j,0) = j;
            cg.at(i,j,0) = i+j;
        }
    }
    Mesh* mesh = new BezierPatch(cg, 16);
    
    Browser::setMesh(mesh);
    Browser::launch();
    return 0;
}