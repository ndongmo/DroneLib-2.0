/*!
* \file Helper.h
* \brief General purpose helper class.
* \author Ndongmo Silatsa
* \date 30-08-2024
* \version 2.0
*/

#pragma once

#include <string>

#define URL_KEY_LENGTH 8

namespace utils
{
/**
 * General purpose helper class.
 */
class Helper {
public:
    /**
	 * @brief Generate random string with the given length.
	 * 
	 * @param length generated string length
     * @return generated string
	 */
    static std::string generateRandomString(int length);
    /**
	 * @brief Generate an url for streaming with random key at the end.
	 * 
	 * @return generated url
	 */
    static std::string generateStreamUrl();
};
}