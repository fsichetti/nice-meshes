#include "Browser.hpp"
#include "Sphere.hpp"
#include "Torus.hpp"
#include "Catenoid.hpp"
#include "BezierPatch.hpp"
#include "Configuration.hpp"
#include "ScalarField.hpp"


int main(int argc, char **argv) {
    // Read configuration file
    std::string file = "./configuration.ini";
    std::string config = "";
    if (argc > 1) {
        config = argv[1];
    }
    if (argc > 2) {
        file = argv[2];
    }

    ConfigManager cm(file, config);
    if (!cm) {
        std::cerr << "Please check your configuration file (" <<
            file << ")" << std::endl;
        return 1;
    }

    // Apply configuration

    // Seed
    if (cm["seed"] != "") RandPoint::seed(std::stoi(cm["seed"]));
    else RandPoint::seed();

    const unsigned int repeat = std::stoi(cm["repeat"]);
    // determines the number of leading zeroes used in mesh names
    const unsigned int repStringLen = std::to_string(repeat-1).length();
    for (unsigned int i = 0; i < repeat; ++i) {
        // Mesh
        Mesh* mesh;
        if (cm["shape"] == "torus") {
            mesh = new Torus(
                std::stoi(cm["samples"]),
                std::stod(cm["outerRadius"]),
                std::stod(cm["innerRadius"])
            );
        }
        else if (cm["shape"] == "catenoid") {
            mesh = new Catenoid(
                std::stoi(cm["samples"]),
                std::stod(cm["outerRadius"]),
                std::stod(cm["innerRadius"])
            );
        }
        else if (cm["shape"] == "sphere") {
            mesh = new Sphere(
                std::stoi(cm["subdivision"]),
                std::stod(cm["radius"])
            );
        }
        else if (cm["shape"] == "bezier") {
            BezierPatch::ControlGrid cg(
                std::stod(cm["radius"]),
                std::stod(cm["borderVariance"]),
                std::stod(cm["innerVariance"])
            );

            if (cm["sampling"] != "") {
                BezierPatch::PlaneSampling smp;
                smp.readFromObj(cm["sampling"]);
                mesh = new BezierPatch(cg, smp);
            }
            else {
                mesh = new BezierPatch(cg,
                    std::stoi(cm["samples"]));
            }
        }
        else {
            std::cerr << "Invalid shape (" <<
                file << ")" << std::endl;
            return 1;
        }
        
        // Name
        if (cm["name"] != "") {
            std::string num;
            if (repeat > 1) {
                // Add leading 0s to the mesh number
                num = std::to_string(i);
                while (num.length() < repStringLen) {
                    num = '0' + num;
                }
            }
            else num = "";
            mesh->name = cm["name"] + num;
        }

        // Processing
        if (cm["centered"] == "true") {
            mesh->makeCentered();
        }
        if (std::stod(cm["noise"]) > 0) {
            const auto t = cm["noiseType"];
            const auto ael = mesh->avgEdgeLength();
            bool nrm, tan;
            nrm = (t == "3d" || t == "normal");
            tan = (t == "3d" || t == "tangential");
            mesh->gaussNoise(ael*std::stod(cm["noise"]), nrm, tan);
        }
        
        // Mode
        if (cm["interactive"] == "true") {
            Trackball trb;
            Browser::init(argv[0], &trb);
            mesh->finalize();
            Browser::setMesh(mesh);
            Browser::setOutPath(cm["outFolder"]);
            Browser::launch();
        }
        else {  // save file
            mesh->finalize(true);
        }
        std::string fname = cm["outFolder"] + mesh->name + ".";
        if (cm["saveOBJ"] == "true") mesh->writeOBJ(fname + "obj");
        if (cm["savePLY"] == "true") mesh->writePLY(fname + "ply");
        if (cm["saveOFF"] == "true") mesh->writeOFF(fname + "off");

        // Scalar field
        if (cm["scalarField"] == "true") {
            const float f = TWOPI * std::stof(cm["scalarFrequency"]);
            const float a = std::stof(cm["scalarAmplitude"]);
            ScalarField sf(mesh);

            // Compute values
            const unsigned int vn = mesh->vertNum();
            for(unsigned int i = 0; i < vn; ++i) {
                const float u = mesh->cAttrib(i, Mesh::Attribute::U);
                const float v = mesh->cAttrib(i, Mesh::Attribute::V);
                const float res = a * sin(f * u) * sin(f * v);
                sf.addValue(res);
            }

            // Write
            sf.write(cm["outFolder"] + mesh->name + "Scalar.txt");
        }
        
        delete mesh;
    }
    return 0;
}

