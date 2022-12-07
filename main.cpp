#include "Browser.hpp"
#include "Torus.hpp"
#include "Catenoid.hpp"
#include "BezierPatch.hpp"

int main(int argc, char **argv) {
    Trackball trb;
    // Create GUI
    Browser::init(argv[0], &trb);

    // Random seed
    std::cout << "RNG seed: " << RandPoint::seed() << std::endl;

    // Create mesh
    // Mesh* mesh = new Torus(100, 2, 1);
    // Mesh* mesh = new Catenoid(100, 2, 1);

    BezierPatch::PlaneSampling smp;
    smp.readFromObj("./plane_anisotropic.obj");
    BezierPatch::ControlGrid cg(4, 2.5, 6.5);
    Mesh* mesh = new BezierPatch(cg, smp);
    
    mesh->makeCentered();

    // const auto ael = mesh->avgEdgeLength();
    // mesh->gaussNoise(ael / 32, true, true);

    mesh->finalize();
    Browser::setMesh(mesh);
    Browser::launch();
    return 0;
}