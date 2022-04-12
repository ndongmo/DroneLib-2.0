/*!
* \file JsonParser.h
* \brief Json parser class.
* \author Ndongmo Silatsa Fabrice
* \date 09-03-2019
* \version 1.0
*/

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <map>
#include <string>

/*!
 *  Json parser class.
 */
class JsonParser
{
public:
	/*!
	 * Add new key -> value json item.
	 * \param key: the attribute key to add
	 * \param value: the attribute value to add
	 * \return the current JsonParser object
	 */
	JsonParser& add(const std::string& key, const std::string& value);

	/*!
	 * Get the value related to the given key attribute.
	 * \param key: the attribute key to get the value
	 * \return the string value related to the given key, otherwise an empty string
	 */
	std::string get(const std::string& key);

	/*!
	 * Decode the given json string.
	 * \param json: json string to decode
	 * \return -1 when an error occurred, otherwise 1
	 */
	int decode(char* json);

	/*!
	 * Build and return the json string with the current data mapping.
	 * \return the json string of the current data mapping
	 */
	std::string encode();

private:
	/*!
 	 *  data mapping key -> value
 	 */
	std::map<std::string, std::string> m_data;
};

#endif //JSONPARSER_H