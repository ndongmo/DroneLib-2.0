/*!
* \file Stream.h
* \brief Stream manager service class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "Service.h"
#include "Stream.h"
#include "StreamPool.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace net {
class StreamFragment;
}

using namespace net;

namespace stream
{

/**
 * Stream manager service.
 * Handle incoming udp stream frames.
 */
class StreamReceiver: public Service, public IStreamListener
{
public:
	int begin() override;
	void start() override;
	int end() override;

	void updateFrame() override;

	/**
	* Handle an incoming stream fragment.
	* \param streamFragment new incoming stream fragment
	* \return pointer to the ack data, otherwise null
	*/
	UINT8* newFragment(const StreamFragment &streamFragment);

protected:	
	void run() override;

	/** Old frame index */
	int m_oldFrame = -1;
	/** Current frame index */
	int m_currentFrame = -1;
	/** send ack for each fragment */
	bool m_ackStreamFragment = false;
	/** Receiver mutex */
	std::mutex m_rcvMtx;
	/** Stream pool */
	StreamPool m_pool;
	/** current live stream frame */
	AvStreamFrame m_frame;
};
} // namespace stream