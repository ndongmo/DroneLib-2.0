/*!
* \file Stream.h
* \brief Stream helper class.
* \author Ndongmo Silatsa Fabrice
* \date 12-12-2022
* \version 2.0
*/

#pragma once

#include "net/NetHelper.h"

#include <mutex>

extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
#include <libavcodec/avcodec.h> 
#include <libavformat/avformat.h> 
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h> 
#include <libswresample/swresample.h>
#include <libpostproc/postprocess.h>
}

#define STREAM_FRAME_ACK 18

namespace stream
{

#ifdef av_err2str
#undef av_err2str
#include <string>
av_always_inline std::string av_err2string(int errnum) {
    char str[AV_ERROR_MAX_STRING_SIZE];
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#define av_err2str(err) av_err2string(err).c_str()
#endif  // av_err2str

/**
 * Filter context structure
 */
struct FilteringContext {
    AVFilterContext *buffersink_ctx = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterGraph *filter_graph = NULL;
 
    AVPacket *enc_pkt = NULL;
    AVFrame *filtered_frame = NULL;
};

/**
 * Binary streams structure to transport live stream
 */
struct AvStreamFrame {
	AvStreamFrame() {
		frame = new UINT8[0];
		rcvWindow = new UINT8[STREAM_FRAME_ACK];
		frameSize = 0;
		frameNumber = -1;
		fragmentNumber = -1;
	}
	~AvStreamFrame() {
		delete[] frame;
		delete[] rcvWindow;
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
	/** fragment size */
	int fragmentSize;
    /** index of the current fragment within the frame */
	int fragmentNumber;
};

/**
 * Stream listener class.
 */
class IStreamListener {
public:
	/**
	 * @brief Update the frame buffer with the incoming frame data.
	 */
	virtual void updateFrame() = 0;

	/**
	 * @brief Inform that this frame has already been handled.
	 */
	void frameHandled() {
		m_newFrame = false;
	}

	bool hasNewFrame() const {
		return m_newFrame;
	}
	const UINT8* getData() const {
		return m_data;
	}
	const int getSize() const {
		return m_size;
	}
protected:
	/** New frame is available in the buffer */
	bool m_newFrame = false;
	/** New frame size */
	int m_size = 0;
	/** current stream frame */
	UINT8* m_data = nullptr;
};

} // namespace stream