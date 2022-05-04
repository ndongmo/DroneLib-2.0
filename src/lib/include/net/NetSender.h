/*!
* \file NetSender.h
* \brief Network sender service class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <Service.h>
#include <net/NetUdp.h>
#include <net/NetHelper.h>

#include <thread>
#include <mutex>

namespace net 
{

/*!
 * Network frame sender service.
 */
class NetSender : public Service
{
public:
	int end() override;

	/*!
	 * Sends an ack frame.
	 * \param id frame id
     * \param seq sequence number
	 */
	void sendAck(UINT8 id, UINT8 seq);

    /*!
	 * Send a frame with the given data.
	 * \param id frame id
	 * \param type frame type
	 * \param dataSize data size
	 * \param data data to write on the buffer
	 */
	void sendFrame(int id, int type, int dataSize, UINT8* data);

	/*!
	 * Decode the given format and send a frame with the contained data.
	 * \param id frame id
	 * \param type frame type
	 * \param format data format
	 */
	void sendFrame(int id, int type, const char* format, ...);

protected:    
	/*!
	 * Writes the given frame header parameters on the buffer.
	 * \param id frame id
	 * \param type frame type
	 * \param totalSize frame total size
	 */
	void writeFrameHeader(int id, int type, int totalSize);

    /*!
	 * Returns the length of integers in the given string format.
	 * \param format string format
     * \return length
	 */
	int getLength(const char* format);

	/*!
	 * Returns the next sequence number at the given buffer id.
	 * \param id buffer id
	 * \param seqBuf buffer's sequence
     * \return next sequence number
	 */
	UINT8 getNextSeqID(int id);

	/*! Max fragment size */
	int m_maxFragmentSize;
	/*! Max fragment number */
	int m_maxFragmentNumber;

    /*! Buffer's sequence */
	int* m_seqBuf;
    /*! Buffer */
	UINT8* m_buffer;
    /*! Mutex watcher for sending */
	std::mutex m_sendMtx;
    /*! UDP sender socket */
	NetUdp m_sendSocket;
};
} // namespace net