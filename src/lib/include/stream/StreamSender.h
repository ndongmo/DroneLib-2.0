/*!
* \file StreamSender.h
* \brief Stream sender service class.
* \author Ndongmo Silatsa Fabrice
* \date 12-12-2022
* \version 2.0
*/

#pragma once

#include "Service.h"
#include "Stream.h"
#include "net/NetSender.h"
#include "net/NetHelper.h"

#include <thread>
#include <mutex>

using namespace net;

namespace stream
{

/**
 * Generic stream sender service.
 * Send udp stream frames.
 */
class StreamSender: public Service
{
public:
	/**
	 * @brief Construct a new Stream Sender object
	 * 
	 * @param streamID id of the stream
	 * @param sender  network sender reference
	 */
	StreamSender(unsigned int streamID, NetSender &sender);

	int begin() override;
	void start() override;
	void stop() override;
    int end() override;

protected:
    void run() override;

	/**
	 * @brief Send the packet data fragment by fragment through network.
	 */
	virtual void sendPacket();
	
	/** Stream ID */
	unsigned int m_streamID;
	/** Frame index */
	unsigned int m_frameIndex = 0;
	/** */
	int64_t m_previousTick;
	/** Captured file name */
	std::string m_filename;
	/** Main process */
	std::thread m_process;
	/** Network sender reference */
	NetSender &m_sender;
	/** Type of media (audio/video) */
	AVMediaType m_mediaType;
	/** Media packet to send */
	AVPacket *m_packet = NULL;
	/** Media format context */
	AVFormatContext *m_ifmt_ctx = NULL;
};
} // namespace stream