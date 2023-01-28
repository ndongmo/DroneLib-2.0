/*!
* \file Config.h
* \brief Load and keep configuration data.
* \author Ndongmo Silatsa
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#define CONFIG_FILE "config.json"

namespace utils
{
/**
 * Load and keep configuration data.
 */
class Config
{
public:
	/**
	 * Initialize the json data by reading the config file.
	 */
	static void init();

	/**
	 * Check if the config file exists.
	 * @return true if the config file exists, false otherwise
	 */
	static bool exists();

	/**
	 * Retrieve the requested int value.
	 * \param key the requested attribute name
	 * \param defaultValue returned value in case the key is not found
	 * \return the int value if the key is found, otherwise the default value
	 */
	static int getInt(const char *key, int defaultValue);

	/**
	 * Retrieve the requested int variable from the global buffer.
	 * \param key the requested attribute name
	 * \return the int value if the key is found, otherwise raise a warning and return 0
	 */
	static int getIntVar(const char *key);

	/**
	 * Retrieve the requested int value and set the given reference.
	 * \param key the requested attribute name
	 * \param value reference of the value to set
	 * \return true if the value has been set, false otherwise
	 */
	static bool setInt(const char *key, int &value);

	/**
	 * Set the value of the given int variable in the global buffer.
	 * \param key the requested attribute name
	 * \param value the new value of the variable
	 */
	static void setIntVar(const char *key, int value);

	/**
	 * Retrieve the requested string value.
	 * \param key the requested attribute name
	 * \param defaultValue returned value in case the key is not found
	 * \return the string value if the key is found, otherwise the default value
	 */
	static std::string getString(const char *key, std::string defaultValue);

	/**
	 * Retrieve the requested string variable from the global buffer.
	 * \param key the requested attribute name
	 * \return the int value if the key is found, otherwise raise a warning and return 0
	 */
	static std::string getStringVar(const char *key);

	/**
	 * Retrieve the requested string value and set the given reference.
	 * \param key the requested attribute name
	 * \param value reference of the value to set
	 * \return true if the value has been set, false otherwise
	 */
	static bool setString(const char *key, std::string &value);

	/**
	 * Set the value of the given string variable in the global buffer.
	 * \param key the requested attribute name
	 * \param value the new value of the variable
	 */
	static void setStringVar(const char *key, const std::string &value);

	/**
	 * Retrieve the requested value. String litteral should be cast as std::string
	 * before sending as parameter.
	 * \param key the requested attribute name
	 * \param defaultValue returned value in case the key is not found
	 * \return the stored value if the key is found, otherwise the default value
	 */
	template<typename T>
	static T get(const char *key, T defaultValue) {
		if(m_config.m_json.contains(key)) {
			m_config.m_json[key].get_to(defaultValue);
		}
		return defaultValue;
	}

	/**
	 * Retrieve the requested value and set the given reference.
	 * \param key the requested attribute name
	 * \param value reference of the value to set
	 * \return true if the value has been set, false otherwise
	 */
	template<typename T>
	static bool set(const char *key, T& value) {
		if(m_config.m_json.contains(key)) {
			m_config.m_json[key].get_to(value);
			return true;
		}
		return false;
	}

private:
	/**
	 * Private config constructor.
	 */
	Config();

	/**
	 * Config singleton object.
	 */
	static Config m_config;

	/**
	 * Keep the config initialization state.
	 */
	bool m_init;

	/**
	 * Json instance keeping the file data.
	 */
	nlohmann::json m_json;

	/**
	 * Int data buffer.
	 */
	std::unordered_map<std::string, int> m_intBuffer;

	/**
	 * String data buffer.
	 */
	std::unordered_map<std::string, std::string> m_stringBuffer;
};
}

