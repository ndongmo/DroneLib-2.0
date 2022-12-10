#include "utils/Config.h"

#include <fstream>

namespace utils {

Config Config::m_config;

Config::Config() {
    init();
}

void Config::init() {
    if(!m_config.m_init) {
        std::ifstream input(CONFIG_FILE);

        if(input.fail()) {
            perror(CONFIG_FILE);
            m_config.m_init = false;
        } 
        else {
            input >> m_config.m_json;
            m_config.m_init = true;
        }
        input.close();
    }
}

bool Config::exists() {
    std::ifstream file(CONFIG_FILE);
    bool exist = !file.fail();
    file.close();
    return exist;
}

int Config::getInt(const char *key, int defaultValue) {
    if(m_config.m_json.contains(key)) {
        m_config.m_json[key].get_to(defaultValue);
    }
    return defaultValue;
}

bool Config::setInt(const char *key, int &value) {
    if(m_config.m_json.contains(key)) {
        m_config.m_json[key].get_to(value);
        return true;
    }
    return false;
}

std::string Config::getString(const char *key, std::string defaultValue) {
    if(m_config.m_json.contains(key)) {
        m_config.m_json[key].get_to(defaultValue);
    }
    return defaultValue;
}

bool Config::setString(const char *key, std::string &defaultValue) {
    if(m_config.m_json.contains(key)) {
        m_config.m_json[key].get_to(defaultValue);
        return true;
    }
    return false;
}
    
} // namespace name
