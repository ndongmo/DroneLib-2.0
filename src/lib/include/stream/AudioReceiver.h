/*!
* \file AudioReceiver.h
* \brief Audio stream receiver service class.
* \author Ndongmo Silatsa Fabrice
* \date 12-12-2022
* \version 2.0
*/

#pragma once

#include "StreamReceiver.h"

#include <mutex>
#include <thread>
#include <condition_variable>

namespace stream
{

/**
 * Audio stream receiver service.
 * Send udp Audio stream frames.
 */
class AudioReceiver: public StreamReceiver
{
public:
	AudioReceiver();
	bool isActive() const override;
};
} // namespace stream
