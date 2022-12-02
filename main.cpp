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
    BezierPatch::ControlGrid cg(5);
    Mesh* mesh = new BezierPatch(cg, 80);
    mesh->writeToObj("./bez.obj");
    
    Browser::setMesh(mesh);
    Browser::launch();
    return 0;
}