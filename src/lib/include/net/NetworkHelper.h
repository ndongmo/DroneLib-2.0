/*!
* \file NetworkHelper.h
* \brief writing and reading buffer functions.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <string>

#ifdef _WIN32 
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <arpa/inet.h>

typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
#endif

#define FRAME_HEADER 7

namespace net
{

/*!
 * Network frame data structure.
 */
typedef struct NetworkFrame {
	/*! data type (1 byte) */
	UINT8 type;
	/*! target buffer (1 byte) */
	UINT8 id;
	/*! Sequence number (1 byte) */
	UINT8 seq;
	/*! Total size of the frame */
	UINT32 totalSize;
	/*! Total size of the data */
	UINT32 dataSize;
	/*! Data buffer */
	UINT8 *data;

} NetworkFrame;

/*!
 * Implement utility methods for writing and reading values on a buffer.
 */
class NetworkHelper
{
public:
	/*!
	 * Convert the given IPV4 structure address into a string.
	 * \param sa : IPV4 struct address
	 */
	static std::string getIpv4Addr(struct sockaddr_in &sa);

	/*!
	 * Writes the given UINT32 on the given char* buffer starting to the index.
	 * @param value value to write on the buffer
	 * @param buffer buffer address
	 * @param index starting position to write
	 */
	static void writeUInt32(UINT32 value, UINT8* buffer, int index);

	/*!
	 * Writes the given UINT16 on the given char* buffer starting to the index.
	 * @param value value to write on the buffer
	 * @param buffer buffer address
	 * @param index starting position to write
	 */
	static void writeUInt16(UINT16 value, UINT8* buffer, int index);

	/*!
	 * Writes the given network frame on the given buffer.
	 * \param frame network frame to build
	 * \param buffer raw data buffer
	 */
	static void writeFrame(const NetworkFrame& frame, UINT8* buffer);

	/*!
	 * Writes the given network frame on the given buffer.
	 * \param frame network frame to build
	 * \param buffer raw data buffer to write on
	 * \param format data format
	 */
	static void writeFrame(const NetworkFrame& frame, UINT8* buffer, const char* format, ...);

	/*!
	 * Gets the string from the given char array.
	 * @param buffer buffer address
	 * @param index starting position to write
	 * \return the read string value.
	 */
	static std::string readString(const char *buffer, int index);

	/*!
	 * Build an unsigned int32 by reading
	 * consecutive byte (UINT8) starting to the given index.
	 * @param buffer buffer address
	 * @param index starting position to write
	 * \return the built UINT32.
	 */
	static UINT32 readUInt32(const char *buffer, int index);

	/*!
	 * Build an unsigned int32 by reading
	 * consecutive byte (UINT8) starting to the given index.
	 * @param buffer buffer address
	 * @param index starting position to write
	 * \return the built UINT32.
	 */
	static UINT32 readUInt32(const UINT8 *buffer, int index);

	/*!
	 * Build an unsigned int16 by reading
	 *	consecutive byte (UINT8) starting to the given index.
	 * @param buffer buffer address
	 * @param index starting position to write
	 * \return the built UINT16.
	 */
	static UINT16 readUInt16(const char *buffer, int index);

	/*!
	 * Build an unsigned int16 by reading
	 *	consecutive byte (UINT8) starting to the given index.
	 * @param buffer buffer address
	 * @param index starting position to write
	 * \return the built UINT16.
	 */
	static UINT16 readUInt16(const UINT8 *buffer, int index);

	/*!
	 * Builds the given network frame from the given raw data buffer.
	 * \param buffer : raw data buffer
	 * \param frame : network frame to build
	 */
	static void readFrame(const char* buffer, NetworkFrame& frame);

	/*!
	 * Reads the data frame and updates the given argument's pointer.
	 * \param frame : network frame to build
	 * \param format data format (arguments must be pointers)
	 */
	static void readArgs(const NetworkFrame& frame, const char* format, ...);

private:
	/*!
	 * Writes a network frame header on the given buffer.
	 * \param frame : network frame to build
	 * \param buffer : raw data buffer
	 */
	static void writeFrameHeader(const NetworkFrame& frame, UINT8* buffer);
};
} // namespace utils