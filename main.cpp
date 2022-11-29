#include "Browser.hpp"
#include "Generators.hpp"

int main(int argc, char **argv) {
    Trackball trb;
    // Create GUI
    Browser::init(argv[0], &trb);

    // Create mesh
    Mesh* mesh = generators::regularCatenoid(20, 1, 1);
    // Mesh* mesh = generators::regularTorus(103, 2, 1);
    
    Browser::setMesh(mesh);
    Browser::launch();
    return 0;
}