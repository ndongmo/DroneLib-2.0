#include "utils/Config.h"
#include "utils/Logger.h"

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

int Config::getIntVar(const char *key) {
    if(m_config.m_intBuffer.find(key) != m_config.m_intBuffer.end()) {
        return m_config.m_intBuffer[key];
    }
    logW << "Config: int variable '" << key << "' does not exist in the buffer!" << std::endl;
    return 0;
}

bool Config::setInt(const char *key, int &value) {
    if(m_config.m_json.contains(key)) {
        m_config.m_json[key].get_to(value);
        return true;
    }
    return false;
}

void Config::setIntVar(const char *key, int value) {
    m_config.m_intBuffer[key] = value;
}

std::string Config::getString(const char *key, std::string defaultValue) {
    if(m_config.m_json.contains(key)) {
        m_config.m_json[key].get_to(defaultValue);
    }
    return defaultValue;
}

std::string Config::getStringVar(const char *key) {
    if(m_config.m_stringBuffer.find(key) != m_config.m_stringBuffer.end()) {
        return m_config.m_stringBuffer[key];
    }
    logW << "Config: string variable '" << key << "' does not exist in the buffer!" << std::endl;
    return "";
}

bool Config::setString(const char *key, std::string &defaultValue) {
    if(m_config.m_json.contains(key)) {
        m_config.m_json[key].get_to(defaultValue);
        return true;
    }
    return false;
}

void Config::setStringVar(const char *key, const std::string &value) {
    m_config.m_stringBuffer[key] = value;
}
    
} // namespace name
