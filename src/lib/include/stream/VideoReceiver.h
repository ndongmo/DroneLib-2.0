/*!
* \file VideoReceiver.h
* \brief Video stream receiver service class.
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
 * Video decoder structure.
 */
struct VideoDecoder {
	const AVCodec *codec = NULL;
	AVCodecContext *codecCtx = NULL;
	AVFrame *frame = NULL;
	AVFrame *frameScl = NULL;
	AVPacket *avPacket = NULL;
	SwsContext *convertCtx = NULL;

	// not really needed: for compilation purpose
	SwrContext *swr_ctx = NULL;
	AVFilterGraph *filter_graph = NULL;
};

/**
 * Video stream receiver service.
 * Send udp video stream frames.
 */
class VideoReceiver: public StreamReceiver
{
public:
	int begin() override;
	void start() override;
    int end() override;
	void updateFrame() override;

private:	
	void run() override;
	
	/** new network frame arrived */
	bool m_newNetworkFrame;
	/** Stream mutex */
	std::mutex m_streamMtx;
	/** Condition variable for controlling access to the mutex */
    std::condition_variable m_cv;
	/** Main process */
	std::thread m_process;
	/** Video decoder object */
	VideoDecoder m_decoder;
};
} // namespace stream
