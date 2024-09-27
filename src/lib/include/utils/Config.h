/*!
* \file Config.h
* \brief Load and keep configuration data.
* \author Ndongmo Silatsa
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <string>
#include <initializer_list>
#include <unordered_map>

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
	 * Initialize the json data by reading the config file.
	 * 
	 * @param filePath the app file path
	 */
	static void initWithFilePath(const std::string& filePath);

	/**
	 * Close the config file.
	 */
	static void close();

	/**
	 * Check if the config file exists.
	 * @return true if the config file exists, false otherwise
	 */
	static bool exists();

	/**
	 * Check if the config file exists.
	 * 
	 * @param filePath the app file path
	 * @return true if the config file exists, false otherwise
	 */
	static bool existsFilePath(const std::string& filePath);

	/**
	 * Retrieve the requested int variable from the global buffer.
	 * \param key the requested attribute name
	 * \return the int value if the key is found, otherwise raise a warning and return 0
	 */
	static int getInt(const char *key);

	/**
	 * Retrieve the requested string key from the global buffer.
	 * \param key the requested attribute name
	 * \return the int value if the key is found, otherwise raise a warning and return 0
	 */
	static std::string getString(const char *key);

	/**
	 * Set the value of the given int key in the global buffer.
	 * \param key the requested attribute name
	 * \param value the new value of the key
	 */
	static void setInt(const char *key, int value);

	/**
	 * Set the value of the given int key in the global buffer only if 
	 * it is not already set.
	 * \param key the requested attribute name
	 * \param value the new value of the key
	 */
	static void setIntDefault(const char *key, int value);

	/**
	 * Set the value of the given string key in the global buffer.
	 * \param key the requested attribute name
	 * \param value the new value of the key
	 */
	static void setString(const char *key, const std::string &value);

	/**
	 * Set the value of the given string key in the global buffer only if
	 * it is not already set.
	 * \param key the requested attribute name
	 * \param value the new value of the key
	 */
	static void setStringDefault(const char *key, const std::string &value);

	/**
	 * @brief Encode the given list of keys with their values into json string.
	 * The key -> value pairs must have been set beforehand.
	 * 
	 * @param keys the list of keys to encode
	 * @return json string 
	 */
	static std::string encodeJson(const std::initializer_list<const std::string> &keys);

	/**
	 * @brief Decode the given json string and store the couple key -> value into the 
	 * global buffer.
	 * 
	 * @param json the json string to decode
	 * @return int 1 if the decoding was succeed, otherwise -1
	 */
	static int decodeJson(const std::string& json);

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
	 * Int data buffer.
	 */
	std::unordered_map<std::string, int> m_intBuffer;

	/**
	 * String data buffer.
	 */
	std::unordered_map<std::string, std::string> m_stringBuffer;
};
}

