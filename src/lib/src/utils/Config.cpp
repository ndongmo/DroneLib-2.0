#include "utils/Config.h"
#include "utils/Logger.h"

#include <fstream>
#include <regex>

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
            std::string str((std::istreambuf_iterator<char>(input)),
                 std::istreambuf_iterator<char>());
            if(m_config.decodeJson(str.c_str()) == -1) {
                logE << "Config: decoding config file '" << CONFIG_FILE << "' failed" << std::endl;
                m_config.m_init = false;
            } else {
                m_config.m_init = true;
            }
        }
        input.close();
    }
}

void Config::close() {
    m_config.m_init = false;
    m_config.m_intBuffer.clear();
    m_config.m_stringBuffer.clear();
}

bool Config::exists() {
    std::ifstream file(CONFIG_FILE);
    bool exist = !file.fail();
    file.close();
    return exist;
}

int Config::getInt(const char *key) {
    if(m_config.m_intBuffer.find(key) != m_config.m_intBuffer.end()) {
        return m_config.m_intBuffer[key];
    }
    logW << "Config: int variable '" << key << "' does not exist in the buffer!" << std::endl;
    return 0;
}

std::string Config::getString(const char *key) {
    if(m_config.m_stringBuffer.find(key) != m_config.m_stringBuffer.end()) {
        return m_config.m_stringBuffer[key];
    }
    logW << "Config: string variable '" << key << "' does not exist in the buffer!" << std::endl;
    return "";
}

void Config::setInt(const char *key, int value) {
    m_config.m_intBuffer[key] = value;
}

void Config::setIntDefault(const char *key, int value) {
    if(m_config.m_intBuffer.find(key) == m_config.m_intBuffer.end()) {
        m_config.m_intBuffer[key] = value;
    }
}

void Config::setString(const char *key, const std::string &value) {
    m_config.m_stringBuffer[key] = value;
}

void Config::setStringDefault(const char *key, const std::string &value) {
    if(m_config.m_stringBuffer.find(key) == m_config.m_stringBuffer.end()) {
        m_config.m_stringBuffer[key] = value;
    }
}

std::string Config::encodeJson(const std::initializer_list<const std::string> &keys) {
    std::string json = "{";
    for(const std::string &key : keys) {
        std::string value;
        if(m_config.m_stringBuffer.find(key) != m_config.m_stringBuffer.end()) {
            value = "\"" + m_config.m_stringBuffer[key] + "\"";
        } else if(m_config.m_intBuffer.find(key) != m_config.m_intBuffer.end()) {
            value = std::to_string(m_config.m_intBuffer[key]);
        } else {
            logW << "Config: variable '" << key << "' does not exist in the buffer!" << std::endl;
            continue;
        }
        json += "\"" + key + "\" : " + value + ",";
    }
    json.replace(json.end() - 1, json.end(), "}");

    return json;
}

int Config::decodeJson(const std::string& json) {
    int err = 1;
    bool isString;
	std::regex  regex("\"(.*?)\"|:\\s*([0-9-]+)\\s*,\\s*|:\\s*([0-9-]+)\\s*\\}");
	auto words_begin = std::sregex_iterator(json.begin(), json.end(), regex);
	auto words_end = std::sregex_iterator();

	try {
		for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
			std::smatch match = *i++;
			std::string key = match.str();
			key.erase(0, 1);
			key.erase(key.length() - 1);

			match = *i;
			std::string value = match.str();
            isString = value[0] == '"';
			value.erase(0, 1);
			value.erase(value.length() - 1);

            if(isString) {
                m_config.m_stringBuffer[key] = value;
            } else {
                m_config.m_intBuffer[key] = std::stoi(value);
            }
		}
	}
	catch (...) {
        logW << "Config: decoding json failed -> " << json << std::endl;
		err = -1;
	}
	return err;
}
    
} // namespace name
