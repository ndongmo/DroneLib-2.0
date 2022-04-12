/*!
* \file Utils.h
* \brief Implements writing and reading buffer functions.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#ifndef UTILS_H
#define UTILS_H

#include <string>

#ifdef _WIN32 
#include <WS2tcpip.h>
#else
#include <sys/types.h>

typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;

#endif

class Utils
{
public:
	/*!
	* \brief Writes the given UINT32 on the given char* buffer starting to the index.
	*/
	static void writeUInt32(UINT32 value, UINT8* byte, int index);
	/*!
	* \brief Writes the given UINT16 on the given char* buffer starting to the index.
	*/
	static void writeUInt16(UINT16 value, UINT8* byte, int index);
	/*!
	* \brief Gets the string from the given char array.
	* \return string.
	*/
	static std::string readString(char *byte, int index);
	/*!
	* \brief Build an unsigned int32 by reading
	*	consecutive byte (UINT8) starting to the given index.
	* \return the builded UINT32.
	*/
	static UINT32 readUInt32(UINT8 *byte, int index);
	static UINT32 readUInt32(char *byte, int index);
	/*!
	* \brief Build an unsigned int16 by reading
	*	consecutive byte (UINT8) starting to the given index.
	* \return the builded UINT16.
	*/
	static UINT16 readUInt16(UINT8 *byte, int index);
	static UINT16 readUInt16(char *byte, int index);
};

#endif //UTILS_H