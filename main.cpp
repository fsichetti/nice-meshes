#include "Browser.hpp"
#include "Sphere.hpp"
#include "Torus.hpp"
#include "Catenoid.hpp"
#include "BezierPatch.hpp"
#include "Configuration.hpp"
#include "VectorField.hpp"

void runConfig(char* pname, std::string fname, std::string cname, bool paral);


int main(int argc, char **argv) {
    // Read configuration file
    std::string filename = "./configuration.ini";
    std::vector<std::string> configs;
    bool firstarg = true;
    bool parallel = false;
    // Parse arguments
    for (unsigned int a = 1; a < argc; ++a) {
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

    const unsigned int confCount = std::max(1, argc - 2);
    
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

    const unsigned int repeat = std::stoi(cm["repeat"]);
    // determines the number of leading zeroes used in mesh names
    const unsigned int repStringLen = std::to_string(repeat-1).length();
    for (unsigned int i = 0; i < repeat; ++i) {
        // Mesh
        Mesh* mesh;
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
                BezierPatch::ControlGrid *cg = new BezierPatch::ControlGrid(
                    std::stod(cm["radius"]),
                    std::stod(cm["borderVariance"]),
                    std::stod(cm["innerVariance"])
                );

                if (cm["inputOBJ"] != "") {
                    PlaneSampling smp(cm["inputOBJ"]);
                    mesh = new BezierPatch(cg, smp);
                }
                else {
                    mesh = new BezierPatch(cg,
                        std::stoi(cm["samples"]));
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
            ScalarField sf(mesh, 2);
            

            const unsigned int vn = mesh->vertNum();
            // Compute field
            for(unsigned int i = 0; i < vn; ++i) {
                const double u = mesh->cAttrib(i, Mesh::Attribute::U);
                const double v = mesh->cAttrib(i, Mesh::Attribute::V);
                const double freqpi = TWOPI * freq;
                double f = ampl *
                    sin(freqpi * u) * sin(freqpi * v);
                double fu = ampl * freqpi * 
                    cos(freqpi * u) * sin(freqpi * v);
                double fv = ampl * freqpi * 
                    sin(freqpi * u) * cos(freqpi * v);
                double fuu = -pow(freqpi, 2) * f;
                double fuv = pow(freqpi, 2) * ampl *
                    cos(freqpi * u) * cos(freqpi * v);
                double fvv = fuu;
                    

                if (cm["shape"] == "sphere") {
                    // Additional factor that goes to zero at the poles
                    const double x = pow(2 * v - 1, 2);
                    const double p = - 6*pow(x,5) + 15*pow(x,4)
                        - 10*pow(x,3) + 1;
                    const double pv = (- 30*pow(x,4) + 60*pow(x,3)
                        - 30*pow(x,2)) * (8*v - 4);
                    const double pvv = (- 120*pow(x,3) + 180*pow(x,2)
                        - 60*x) * pow(8*v - 4, 2) + (- 30*pow(x,4)
                        + 60*pow(x,3) - 30*pow(x,2)) * 8;

                    // Original values of f
                    const double of = f, ofu = fu, ofv = fv,
                        ofuu = fuu, ofuv = fuv, ofvv = fvv;

                    // Product rule
                    f = of * p;
                    fu = ofu * p;
                    fv = ofv * p + of * pv;
                    fuu = ofuu * p;
                    fuv = ofuv * p + ofu * pv;
                    fvv = ofvv * p + 2 * ofv * pv + of * pvv;
                }

                sf.setValue(f, i, 0, 0);
                sf.setValue(fu, i, 1, 0);
                sf.setValue(fv, i, 0, 1);
                sf.setValue(fuu, i, 2, 0);
                sf.setValue(fuv, i, 1, 1);
                sf.setValue(fvv, i, 0, 2);
            }
            // Write
            const bool head = (cm["scalarHeader"] == "true");
            sf.write(cm["outFolder"] + mesh->name + "Scalar.txt", head);

            // Compute differential quantities
            const bool lap = cm["scalarLaplacian"] == "true";
            const bool gra = cm["scalarGradient"] == "true";
            const bool hes = cm["scalarHessian"] == "true";
            const bool euv = cm["exportUV"] == "true";
            if (lap || gra || hes || euv) {
                ScalarField *laplacian;
                VectorField *gradient, *hessian, *uvfield;

                // Create
                if (lap) laplacian = new ScalarField(mesh);
                if (gra) gradient = new VectorField(mesh);
                if (hes) hessian = new VectorField(mesh);
                if (euv) uvfield = new VectorField(mesh);
                
                // Compute
                for(unsigned int i = 0; i < vn; ++i) {
                    const double u = mesh->cAttrib(i, Mesh::Attribute::U);
                    const double v = mesh->cAttrib(i, Mesh::Attribute::V);
                    const double f = sf.getValue(i, 0, 0);
                    const double fu = sf.getValue(i, 1, 0);
                    const double fv = sf.getValue(i, 0, 1);
                    const double fuu = sf.getValue(i, 2, 0);
                    const double fuv = sf.getValue(i, 1, 1);
                    const double fvv = sf.getValue(i, 0, 2);
                    
                    if (lap) laplacian->setValue(
                        mesh->laplacian(u, v, f, fu, fv, fuu, fuv, fvv), i);
                    if (gra) gradient->setValue(
                        mesh->gradient(u, v, f, fu, fv), i);
                    if (hes) hessian->setValue(
                        mesh->hessian(u, v, f, fu, fv, fuu, fuv, fvv), i);
                    if (euv) uvfield->setValue(glm::vec3(u,v,0), i);
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
        }
        delete mesh;
    }
}