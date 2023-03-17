#include "Configuration.hpp"

void ConfigManager::defaultValues() {
    auto& c = configuration;

    c["shape"] = "torus";
    c["name"] = "mesh";

    c["interactive"] = "true";
    c["repeat"] = "1";
    c["inputPlane"] = "";
    c["inputShape"] = "";
    c["outFolder"] = "./";
    c["savePLY"] = "false";
    c["saveOBJ"] = "false";
    c["saveOFF"] = "false";
    c["exportUV"] = "false";
    c["exportControlGrid"] = "false";

    c["anisotropy"] = "";
    c["samples"] = "64";
    c["radius"] = "1";
    c["innerRadius"] = "1";
    c["outerRadius"] = "2";
    c["centered"] = "false";
    c["subdivision"] = "3";

    c["noise"] = "0";
    c["noiseType"] = "3d";

    c["seed"] = "";
    c["borderVariance"] = "1";
    c["innerVariance"] = "1";

    c["scalarField"] = "false";
    c["scalarHeader"] = "false";
    c["scalarFrequency"] = "1";
    c["scalarAmplitude"] = "1";
    c["scalarLaplacian"] = "false";
    c["scalarGradient"] = "false";
    c["scalarFaceGradient"] = "false";
    c["scalarHessian"] = "false";
}


ConfigManager::ConfigManager(std::string path, std::string section) {
    std::ifstream ini(path);

    // Parse file
    if (!ini.is_open()) return;
    readOK = true;
    for (char c : section) c = tolower(c);
    defaultValues();
    std::string key = "";
    bool readConf = false;  // reading configuration
    bool globals = true;    // reading globals
    while (!ini.eof()) {
        std::string val;
        ini >> val;
        if (val.empty() || val.front() == ';') {
            // Discard the rest of the line
            std::string a = "";
            std::getline(ini, a);
            continue;
        }
        for (char c : val) c = tolower(c);

        // Check section name against selected configuration
        if (val.front() == '[' && val.back() == ']') {
            globals = false;    // globals end at the first section
            if (readConf) break;    // already read a full config, so stop
            std::string sec = val.substr(1, val.length()-2);
            if (section == "" || section == sec) {
                readConf = true;
                continue;
            }
        }
        // Reading configuration
        else if (readConf || globals) {
            if (key.empty()) key = val;
            else if (val != "=") {
                configuration[key] = val;
                key.clear();
            }
        }
    }
}