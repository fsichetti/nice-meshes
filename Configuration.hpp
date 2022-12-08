#include <fstream>
#include <map>

class ConfigManager {
    public:
        ConfigManager(std::string path = "configuration.ini",
            std::string selection = "");

        operator bool() { return readOK; }
        std::string operator[] (const char* s) { return configuration.at(s); }
    private:
        bool readOK = false;
        std::map<std::string, std::string> configuration;
        
        void defaultValues();
};