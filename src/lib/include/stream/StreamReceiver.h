/*!
* \file Stream.h
* \brief Stream manager service class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "Service.h"
#include "StreamPool.h"
#include "net/NetUdp.h"
#include "net/NetHelper.h"

#include <thread>
#include <mutex>

using namespace net;

#define STREAM_FRAME_ACK 18

namespace stream
{

/**
 * Fragment data structure.
 */
typedef struct Fragment {
	Fragment() {
		size = 0;
		data = new UINT8[1];
	}
    /** fragment data */
	UINT8* data;
    /** fragment size */
	int size;

} Fragment;

/**
 * Acknowledgement frame data structure.
 */
typedef struct AckFrame {
    /** frame number to ack */
	UINT16 frameNumber;
    /** ack frame */
	UINT8* frameACK;
} AckFrame;

/**
 * Binary streams structure to transport live audio and video data
 */
typedef struct AvStreamFrame {
	AvStreamFrame() {
		frameSize = 0;
		frame = new UINT8[0];
		rcvWindow = new UINT8[STREAM_FRAME_ACK];
		frameNumber = -1;
		fragmentPerFrame = 1;
		fragmentNumber = -1;
	}
    /** rcv window and frame ack pointer */
	/** frameNumber (2bytes) + rcvWindow (16bytes) */
	UINT8* rcvWindow;
    /** current frame */
	UINT8 *frame;
    /** frame flags */
	UINT8 frameFlags;
    /** frame size */
	int frameSize;
	/** frame number */
	int frameNumber;
    /** total number of fragments within the frame */
	int fragmentPerFrame;
    /** index of the current fragment within the frame */
	int fragmentNumber;
} AvStreamFrame;

/**
 * Stream manager service.
 * Handle incoming udp stream frames.
 */
class StreamReceiver: public Service
{
public:
	void start() override;
    int end() override;

private:
    void run() override;

	/**
	* Handle an incoming frame.
	* \param netFrame new incoming frame
	* \return pointer to the ack data, otherwise NULL
	*/
	UINT8* newFrame(const NetFrame& netFrame);
	
	/* send ack for each fragment */
	bool m_ackStreamFragment;
	/** Receiver mutex */
	std::mutex m_rcvMtx;
	/** Stream pool */
	StreamPool m_pool;
	/** current live stream frame */
	AvStreamFrame m_avFrame;
};
} // namespace stream