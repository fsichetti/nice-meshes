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
    std::vector<std::string> configs;
    if (argc == 2) {
        configs.push_back(argv[1]);
    }
    else if (argc > 2) {
        file = argv[1];
        for (unsigned int c = 2; c < argc; ++c) { configs.push_back(argv[c]); }
    }
    if (configs.size() == 0) { configs.push_back(""); }

    const unsigned int confCount = std::max(1, argc - 2);
    for (std::string config : configs) {
        ConfigManager cm(file, config);
        if (!cm) {
            std::cerr << "Please check your configuration file (" <<
                file << ") for config " << config << std::endl;
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
                const float freq = std::stof(cm["scalarFrequency"]);
                const float ampl = std::stof(cm["scalarAmplitude"]);
                ScalarField sf(mesh);

                const unsigned int vn = mesh->vertNum();
                // Compute field
                for(unsigned int i = 0; i < vn; ++i) {
                    const float u = mesh->cAttrib(i, Mesh::Attribute::U);
                    const float v = mesh->cAttrib(i, Mesh::Attribute::V);
                    float z = 1;
                    if (cm["shape"] == "sphere") {
                        const float x = pow(2*u - 1, 2);
                        z = 1 - 10*pow(x,3) + 15*pow(x,4) - 6*pow(x,5);
                    }
                    const float res = ampl * sin(TWOPI * freq * u)
                         * sin(TWOPI * freq * v) * z;
                    sf.addValue(res);
                }
                // Write
                const bool head = (cm["scalarHeader"] == "true");
                sf.write(cm["outFolder"] + mesh->name + "Scalar.txt", head);

                // Compute laplacian
                if (cm["scalarLaplacian"] == "true") {
                    ScalarField lap(mesh);
                    
                    for(unsigned int i = 0; i < vn; ++i) {
                        const float u = mesh->cAttrib(i, Mesh::Attribute::U);
                        const float v = mesh->cAttrib(i, Mesh::Attribute::V);
                        float res = 0;
                        if (cm["shape"] == "sphere") {
                        }
                        else if (cm["shape"] == "torus") {
                            const float val = sf.getValue(i);
                            const float r = std::stof(cm["innerRadius"]);
                            const float R = std::stof(cm["outerRadius"]);
                            const float d = R + r * cos(TWOPI * v);
                            res = -freq * (sin(TWOPI * v)/(d*r) * ampl *
                                sin(TWOPI * freq * u) * cos(TWOPI * freq * v) +
                                freq * val * (1/pow(d,2) + 1/pow(r,2)));
                        }
                        else if (cm["shape"] == "catenoid") {
                            const float val = sf.getValue(i);
                            const float r = std::stof(cm["innerRadius"]);
                            const float R = std::stof(cm["outerRadius"]);
                            const float c = cosh((2*v-1)*acosh(R/r));
                            res = -(1 + 1/pow(r,2)) * pow(freq,2)*val/pow(c,2);
                        }
                        else if (cm["shape"] == "bezier") {
                        }
                        lap.addValue(res);
                    }
                    lap.write(cm["outFolder"] + mesh->name + "Laplacian.txt",
                        head);
                }
            }
            
            delete mesh;
        }
    }
    return 0;
}

