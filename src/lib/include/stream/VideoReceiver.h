/*!
* \file VideoReceiver.h
* \brief Video stream receiver service class.
* \author Ndongmo Silatsa Fabrice
* \date 12-12-2022
* \version 2.0
*/

#pragma once

#include "StreamReceiver.h"

namespace stream
{

/**
 * Video stream receiver service.
 * Send udp video stream frames.
 */
class VideoReceiver: public StreamReceiver
{
public:
	VideoReceiver();
	bool isActive() const override;
};
} // namespace stream
