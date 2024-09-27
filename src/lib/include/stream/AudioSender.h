/*!
* \file AudioSender.h
* \brief Audio Stream sender service class.
* \author Ndongmo Silatsa Fabrice
* \date 02-02-2023
* \version 2.0
*/

#pragma once

#include "StreamSender.h"

namespace stream
{

/**
 * Audio stream sender service.
 * Send udp audio stream frames.
 */
class AudioSender: public StreamSender
{
public:
	/**
	 * @brief Construct a new Audio Stream Sender object
	 * 
	 * @param sender  network sender reference
	 */
	AudioSender(NetSender &sender);

	int begin() override;
	bool isActive() const override;
	std::string toString() override;
	void initEncoderCtx() override;
	int initFilterCtx() override;

private:
	/**
	 * @brief Compute and return the number of samples per channel
	 * 
	 * @param nb_channels number of audio channel
	 * @param format audio format
	 * @return number of samples per channel
	 */
	int getNbSamples(int nb_channels, int format);
};
} // namespace stream