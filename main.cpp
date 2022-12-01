#include "Browser.hpp"
#include "Torus.hpp"
#include "Catenoid.hpp"

int main(int argc, char **argv) {
    Trackball trb;
    // Create GUI
    Browser::init(argv[0], &trb);

    // Create mesh
    Mesh* mesh = new Catenoid(100, 2, 1);
    // Mesh* mesh = new Torus(100, 2, 1);
    
    Browser::setMesh(mesh);
    Browser::launch();
    return 0;
}