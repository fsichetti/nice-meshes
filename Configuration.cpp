#include "Configuration.hpp"

void ConfigManager::defaultValues() {
    auto& c = configuration;
    c["shape"] = "torus";
    c["name"] = "mesh";

    c["mode"] = "interactive";

    c["samples"] = "64";
    c["innerRadius"] = "1";
    c["outerRadius"] = "2";
    c["centered"] = "no";

    c["noise"] = "0";
    c["noiseType"] = "3d";

    c["seed"] = "";
    c["borderVariance"] = "1";
    c["innerVariance"] = "1";
    c["sampling"] = "";
}


ConfigManager::ConfigManager(std::string path, std::string section) {
    std::ifstream ini(path);

    // Parse file
    if (!ini.is_open()) return;
    readOK = true;
    for (char c : section) c = tolower(c);
    defaultValues();
    std::string key = "";
    bool readConf = false;
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
            if (readConf) break;    // already read a full config, so stop
            std::string sec = val.substr(1, val.length()-2);
            if (section == "" || section == sec) {
                readConf = true;
                continue;
            }
        }
        // Reading configuration
        else if (readConf) {
            if (key.empty()) key = val;
            else if (val != "=") {
                configuration[key] = val;
                key.clear();
            }
        }
    }
}