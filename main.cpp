#include "Browser.hpp"
#include "Sphere.hpp"
#include "Torus.hpp"
#include "Catenoid.hpp"
#include "BezierPatch.hpp"
#include "Configuration.hpp"
#include "VectorField.hpp"
#include "SinProductSF.hpp"

void runConfig(char* pname, std::string fname, std::string cname, bool paral);


int main(int argc, char **argv) {
    // Read configuration file
    std::string filename = "./configuration.ini";
    std::vector<std::string> configs;
    bool firstarg = true;
    bool parallel = false;
    // Parse arguments
    for (uint a = 1; a < argc; ++a) {
        if (argv[a][0] == '-') {
            switch (argv[a][1]) {
                case 'p':
                    parallel = true;
                default:
                    continue;
            }
        }
        else {
            if (firstarg) { filename = argv[a]; firstarg = false; }
            else configs.push_back(argv[a]);
        }
    }

    if (configs.size() == 0) {
        // If no configs are specified, run them all
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Please check your configuration file (" <<
                filename << std::endl;
            return 1;
        }
        while (!file.eof()) {
            std::string line;
            file >> line;
            if (line.front() == '[' && line.back() == ']') {
                configs.push_back(line.substr(1, line.length()-2));
            }
        }
        file.close();
    }

    std::cout << "Running configuration(s): ";
    for (std::string c : configs) std::cout << c << " ";
    std::cout << std::endl;

    const uint confCount = std::max(1, argc - 2);
    
    if (parallel) {
        #pragma omp parallel for
        for (std::string config : configs) {
            runConfig(argv[0], filename, config, true);
        }
    }
    else {
        for (std::string config : configs) {
            runConfig(argv[0], filename, config, false);
        }
    }
    
    return 0;
}


void runConfig(char* pname, std::string fname, std::string cname,
    bool parallel) {
    ConfigManager cm(fname, cname);
    if (!cm) {
        std::cerr << "Please check your configuration file (" <<
            fname << ") for config " << cname << std::endl;
        return;
    }

    // Apply configuration

    // Seed
    if (cm["seed"] != "") RandPoint::seed(std::stoi(cm["seed"]));
    else RandPoint::seed();

    const uint repeat = std::stoi(cm["repeat"]);
    // determines the number of leading zeroes used in mesh names
    const uint repStringLen = std::to_string(repeat-1).length();
    for (uint i = 0; i < repeat; ++i) {
        // Mesh
        Mesh *mesh = nullptr;
        BezierPatch::ControlGrid *cg = nullptr;
        try {
            if (cm["shape"] == "torus") {
                if (cm["inputOBJ"] == "") {
                    mesh = new Torus(
                        std::stoi(cm["samples"]),
                        std::stod(cm["outerRadius"]),
                        std::stod(cm["innerRadius"])
                    );
                }
                else {
                    mesh = new Torus(
                        cm["inputOBJ"],
                        std::stod(cm["outerRadius"]),
                        std::stod(cm["innerRadius"])
                    );
                }
            }
            else if (cm["shape"] == "catenoid") {
                if (cm["inputOBJ"] == "") {
                    mesh = new Catenoid(
                        std::stoi(cm["samples"]),
                        std::stod(cm["outerRadius"]),
                        std::stod(cm["innerRadius"])
                    );
                }
                else {
                    mesh = new Catenoid(
                        cm["inputOBJ"],
                        std::stod(cm["outerRadius"]),
                        std::stod(cm["innerRadius"])
                    );
                }
            }
            else if (cm["shape"] == "sphere") {
                if (cm["inputOBJ"] == "") {
                    mesh = new Sphere(
                        std::stoi(cm["subdivision"]),
                        std::stod(cm["radius"])
                    );
                }
                else {
                    mesh = new Sphere(
                        cm["inputOBJ"],
                        std::stod(cm["radius"])
                    );
                }
            }
            else if (cm["shape"] == "bezier") {
                cg = new BezierPatch::ControlGrid(
                    std::stod(cm["radius"]),
                    std::stod(cm["borderVariance"]),
                    std::stod(cm["innerVariance"])
                );

                if (cm["inputOBJ"] != "") {
                    PlaneSampling smp(cm["inputOBJ"]);
                    mesh = new BezierPatch(cg, smp);
                }
                else if (cm["anisotropy"] == "") {
                    mesh = new BezierPatch(cg,
                        std::stoi(cm["samples"]));
                }
                else {
                    mesh = new BezierPatch(cg, 
                        std::stoi(cm["samples"]), std::stod(cm["anisotropy"]));
                }
            }
            else {
                std::cerr << "Invalid shape (" <<
                    fname << ")" << std::endl;
                delete mesh;
                continue;
            }
        }
        catch (Mesh::FileOpenException e) {
            std::cerr << e.what() << " (conf:" << cname << ')' << std::endl;
            delete mesh;
            continue;
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

        // Write cg
        if (cm["exportControlGrid"] == "separate" && cg) {
            std::string basename = cm["outFolder"] + mesh->name;
            cg->writeCoordinate(basename + "x.txt", 0);
            cg->writeCoordinate(basename + "y.txt", 1);
            cg->writeCoordinate(basename + "z.txt", 2);
        }


        // Processing
        if (cm["centered"] == "true") {
            mesh->makeCentered();
        }
        if (std::stod(cm["noise"]) > 0) {
            const auto t = cm["noiseType"];
            const auto ael = mesh->getAverageEdgeLength();
            bool nrm, tan;
            nrm = (t == "3d" || t == "normal");
            tan = (t == "3d" || t == "tangential");
            const double variance = sqrt(ael * std::stod(cm["noise"]));
            mesh->gaussNoise(variance, nrm, tan);
        }
        
        // Mode
        if (cm["interactive"] == "true" && !parallel) {
            Trackball trb;
            Browser::init(pname, &trb);
            mesh->finalize();
            Browser::setMesh(mesh);
            Browser::setOutPath(cm["outFolder"]);
            Browser::launch();
        }
        else {  // save file
            mesh->finalize(true);
            if (cm["interactive"] == "true") {
                std::cerr << "Cannot run interactively in parallel (" <<
                    cname << ")" << std::endl;
            }
        }
        std::string fname = cm["outFolder"] + mesh->name + ".";
        if (cm["saveOBJ"] == "true") mesh->writeOBJ(fname + "obj");
        if (cm["savePLY"] == "true") mesh->writePLY(fname + "ply");
        if (cm["saveOFF"] == "true") mesh->writeOFF(fname + "off");

        // Scalar field
        if (cm["scalarField"] == "true") {
            const double freq = std::stof(cm["scalarFrequency"]);
            const double ampl = std::stof(cm["scalarAmplitude"]);
            SinProductSF signal(mesh, freq, ampl, (cm["shape"] == "sphere"));

            // Write
            const bool head = (cm["scalarHeader"] == "true");
            signal.write(cm["outFolder"] + mesh->name + "Scalar.txt", head);

            // Compute differential quantities
            const bool lap = cm["scalarLaplacian"] == "true";
            const bool gra = cm["scalarGradient"] == "true";
            const bool hes = cm["scalarHessian"] == "true";
            const bool euv = cm["exportUV"] == "true";
            if (lap || gra || hes || euv) {
                ScalarField *laplacian = nullptr;
                VectorField *gradient = nullptr, *hessian = nullptr,
                    *uvfield = nullptr;

                // Create
                if (lap) laplacian = new ScalarField(mesh);
                if (gra) gradient = new VectorField(mesh);
                if (hes) hessian = new VectorField(mesh);
                if (euv) uvfield = new VectorField(mesh);
                
                // Compute
                const uint vn = mesh->vertNum();
                for(uint i = 0; i < vn; ++i) {
                    const double u = mesh->cAttrib(i, Mesh::Attribute::U);
                    const double v = mesh->cAttrib(i, Mesh::Attribute::V);
                    const double f = signal.getValue(i, 0, 0);
                    const double fu = signal.getValue(i, 1, 0);
                    const double fv = signal.getValue(i, 0, 1);
                    const double fuu = signal.getValue(i, 2, 0);
                    const double fuv = signal.getValue(i, 1, 1);
                    const double fvv = signal.getValue(i, 0, 2);
                    
                    
                    if (lap) laplacian->setValue(
                        mesh->laplacian(u, v, f, fu, fv, fuu, fuv, fvv), i);
                    if (gra) gradient->setValue(
                        mesh->gradient(u, v, f, fu, fv), i);
                    if (hes) hessian->setValue(
                        mesh->hessian(u, v, f, fu, fv, fuu, fuv, fvv), i);
                    if (euv) uvfield->setValue(glm::dvec3(u,v,0), i);
                }

                // Write and destroy
                if (lap) {
                    laplacian->write(
                        cm["outFolder"] + mesh->name + "Laplacian.txt",
                        head
                    );
                    delete laplacian;
                }
                if (gra) {
                    gradient->write(
                        cm["outFolder"] + mesh->name + "Gradient.txt",
                        head
                    );
                    delete gradient;
                }
                if (hes) {
                    hessian->write(
                        cm["outFolder"] + mesh->name + "Hessian.txt",
                        head
                    );
                    delete hessian;
                }
                if (euv) {
                    uvfield->write2d(cm["outFolder"] + mesh->name + "UV.txt");
                    delete uvfield;
                }
            }

            // Face diff. quantities
            if (cm["scalarFaceGradient"] == "true") {
                const uint fn = mesh->faceNum();
                VectorField faceGradient(mesh, true);
                for (uint i = 0; i < fn; ++i) {
                    uint f[3] = {
                        mesh->cFacei(i, 0),
                        mesh->cFacei(i, 1),
                        mesh->cFacei(i, 2)
                    };
                    glm::dvec2 centroidUV(0);
                    for (uint ti = 0; ti < 3; ++ti) {
                        const glm::dvec2 uv(
                            mesh->cAttrib(f[ti], Mesh::Attribute::U),
                            mesh->cAttrib(f[ti], Mesh::Attribute::V)
                        );
                        centroidUV += uv;
                    }
                    centroidUV /= glm::dvec1(3);
                    double ff, ffu, ffv, ffuu, ffuv, ffvv;
		            signal.evaluate(centroidUV.x, centroidUV.y,
                        ff, ffu, ffv, ffuu, ffuv, ffvv);
                    faceGradient.setValue(
                        mesh->gradient(centroidUV.x, centroidUV.y,
                            ff, ffu, ffv), i);
                }
                const bool head = (cm["scalarHeader"] == "true");
                faceGradient.write(cm["outFolder"]
                    + mesh->name + "FaceGradient.txt", head);
            }
        }
        delete mesh;
    }
}