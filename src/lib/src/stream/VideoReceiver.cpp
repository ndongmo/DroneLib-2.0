#include "stream/VideoReceiver.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

using namespace utils;

namespace stream {

int VideoReceiver::begin() {
	StreamReceiver::begin();
	
	m_decoder.codec = avcodec_find_decoder((AVCodecID)Config::getIntVar(VIDEO_CODEC));
	if (!m_decoder.codec) {
		logE << "VideoReceiver: video codec not found!" << std::endl;
		return -1;
	}
  
	m_decoder.codecCtx = avcodec_alloc_context3(m_decoder.codec);
	if (!m_decoder.codecCtx) {
		logE << "VideoReceiver: could not allocate video codec context!" << std::endl;
		return -1;
	}
	
	m_decoder.codecCtx->bit_rate = Config::getIntVar(VIDEO_BIT_RATE);
	m_decoder.codecCtx->width = Config::getIntVar(VIDEO_WIDTH);
	m_decoder.codecCtx->height = Config::getIntVar(VIDEO_HEIGHT);
	m_decoder.codecCtx->time_base = (AVRational){1, Config::getIntVar(VIDEO_FPS)};
	m_decoder.codecCtx->framerate = (AVRational){Config::getIntVar(VIDEO_FPS), 1};

	m_decoder.codecCtx->gop_size = 10; /* emit one intra frame every ten frames */
	m_decoder.codecCtx->max_b_frames = 1;
	m_decoder.codecCtx->pix_fmt = (AVPixelFormat)Config::getIntVar(VIDEO_FORMAT);

	if (m_decoder.codec->id == AV_CODEC_ID_H264) {
        av_opt_set(m_decoder.codecCtx->priv_data, "preset", "slow", 0);
	}
	
	int ret = avcodec_open2(m_decoder.codecCtx, m_decoder.codec, NULL);
	if (ret < 0) {
		logE << "VideoReceiver: failed to open video avcodec. " << av_err2str(ret) << std::endl;
		return -1;
	}

	m_decoder.avPacket = av_packet_alloc();
	if (!m_decoder.avPacket) {
		logE << "VideoReceiver: could not allocate packet" << std::endl;
		return -1;
	}

	m_decoder.frame = av_frame_alloc();
	if (!m_decoder.frame) {
		logE << "VideoReceiver: could not allocate video frame" << std::endl;
		return -1;
	}

	m_decoder.frameScl = av_frame_alloc();
	if (!m_decoder.frameScl) {
		logE << "VideoReceiver: could not allocate video frame BGR" << std::endl;
		return -1;
	}

	m_decoder.frame->format = m_decoder.codecCtx->pix_fmt;
	m_decoder.frame->width  = m_decoder.codecCtx->width;
	m_decoder.frame->height = m_decoder.codecCtx->height;

	m_decoder.frameScl->format = (AVPixelFormat)Config::getIntVar(VIDEO_DST_FORMAT);
    m_decoder.frameScl->width  = Config::getIntVar(VIDEO_DST_WIDTH);
    m_decoder.frameScl->height = Config::getIntVar(VIDEO_DST_HEIGHT);

	m_decoder.convertCtx = sws_getContext(m_decoder.codecCtx->width, m_decoder.codecCtx->height, 
		m_decoder.codecCtx->pix_fmt, Config::getIntVar(VIDEO_DST_WIDTH), Config::getIntVar(VIDEO_DST_HEIGHT), 
		(AVPixelFormat)Config::getIntVar(VIDEO_DST_FORMAT), SWS_SPLINE, NULL, NULL, NULL);

	if (!m_decoder.convertCtx) {
		logE << "VideoReceiver: could not create scale context for the conversion" << std::endl;
		return -1;
	}

	if (av_frame_get_buffer(m_decoder.frame, 0) < 0) {
		logE << "VideoReceiver: could not allocate the video frame data" << std::endl;
		return -1;
	}

	if (av_frame_get_buffer(m_decoder.frameScl, 0) < 0) {
		logE << "VideoReceiver: could not allocate the video frame BGR data" << std::endl;
		return -1;
	}

	m_size = av_image_get_buffer_size((AVPixelFormat)Config::getIntVar(VIDEO_DST_FORMAT), 
		Config::getIntVar(VIDEO_DST_WIDTH), Config::getIntVar(VIDEO_DST_HEIGHT), 1);
	m_data = new UINT8[m_size];

	return 1;
}

int VideoReceiver::end() {
	m_running = false;

	m_cv.notify_all();
	if(m_process.joinable()) {
		m_process.join();
	}

	if(m_decoder.codecCtx != NULL) {
		avcodec_free_context(&m_decoder.codecCtx);
		m_decoder.codecCtx = NULL;
	}
	if(m_decoder.convertCtx != NULL) {
		sws_freeContext(m_decoder.convertCtx);
		m_decoder.convertCtx = NULL;
	}
	if(m_decoder.frame != NULL) {
		av_frame_free(&m_decoder.frame);
		m_decoder.frame = NULL;
	}
	if(m_decoder.frameScl != NULL) {
		av_frame_free(&m_decoder.frameScl);
		m_decoder.frameScl = NULL;
	}
	if(m_decoder.avPacket != NULL) {
		av_packet_free(&m_decoder.avPacket);
		m_decoder.avPacket = NULL;
	}
	if(m_decoder.swr_ctx != NULL) {
		swr_free(&m_decoder.swr_ctx);
		m_decoder.swr_ctx = NULL;
	}
	if(m_decoder.filter_graph != NULL) {
		avfilter_graph_free(&m_decoder.filter_graph);
		m_decoder.filter_graph = NULL;
	}
	if(m_data != nullptr) {
		delete[] m_data;
		m_data = nullptr;
	}

	return 1;
}

void VideoReceiver::start() {
	m_running = true;
	m_process = std::thread([this]{run(); });
}

void VideoReceiver::run() {
	int ret;

	while (m_running)
	{
		m_rcvMtx.lock();
		m_oldFrame = m_currentFrame;
		m_rcvMtx.unlock();

		m_decoder.avPacket->data = m_pool.getData(m_oldFrame);
		m_decoder.avPacket->size = m_pool.getSize(m_oldFrame);

		if(m_decoder.avPacket->size > 0) {
			ret = avcodec_send_packet(m_decoder.codecCtx, m_decoder.avPacket);
            if (ret >= 0) {
				while (ret >= 0) {
					ret = avcodec_receive_frame(m_decoder.codecCtx, m_decoder.frame);
					if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
						break;
					} else if (ret < 0) {
						logE << "VideoReceiver: decoding frame failed. " << av_err2str(ret) << std::endl;
						break;
					}

					m_streamMtx.lock();
					{
						sws_scale(m_decoder.convertCtx,
							(const unsigned char* const*)m_decoder.frame->data,
							m_decoder.frame->linesize, 0, m_decoder.codecCtx->height,
							m_decoder.frameScl->data, m_decoder.frameScl->linesize);
						m_newNetworkFrame = true;
					}
					m_streamMtx.unlock();
				}
            } else {
				logE << "VideoReceiver: decoding packet failed. " << av_err2str(ret) << std::endl;
			}
		}

		std::unique_lock<std::mutex> lock(m_rcvMtx);
		m_cv.wait(lock, [this] { return m_oldFrame != m_currentFrame || !m_running; });
	}
}

void VideoReceiver::updateFrame() {
	std::lock_guard<std::mutex> guard(m_rcvMtx);
	if(m_newNetworkFrame) {
		m_newFrame = true;
		m_newNetworkFrame = false;
		memcpy(m_data, m_decoder.frameScl->data, m_size);
	}
}
} // namespace net