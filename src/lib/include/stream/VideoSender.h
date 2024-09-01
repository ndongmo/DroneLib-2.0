/*!
* \file VideoSender.h
* \brief Video Stream sender service class.
* \author Ndongmo Silatsa Fabrice
* \date 02-02-2023
* \version 2.0
*/

#pragma once

#include "StreamSender.h"

namespace stream
{

/**
 * Video stream sender service.
 * Send udp video stream frames.
 */
class VideoSender: public StreamSender
{
public:
	/**
	 * @brief Construct a new Video Stream Sender object
	 * 
	 * @param sender  network sender reference
	 */
	VideoSender(NetSender &sender);

	int begin() override;
	std::string toString() override;
	void initDecoderCtx() override;
	void initEncoderCtx() override;
	int initFilterCtx() override;
};
} // namespace stream