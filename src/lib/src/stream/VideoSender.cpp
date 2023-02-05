#include "stream/VideoSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"


using namespace utils;

namespace stream {

VideoSender::VideoSender(NetSender &sender) : StreamSender(sender) {
	m_mediaType = AVMEDIA_TYPE_VIDEO;
	m_streamID = NS_ID_STREAM_VIDEO;
}

int VideoSender::begin() {
	m_filename = Config::getStringVar(VIDEO_DEVICE);
	
	if(StreamSender::begin() == -1) {
		return -1;
	}

	logI << "VideoSender:" <<
	" fps=" << m_ifmt_ctx->streams[0]->r_frame_rate.num <<
	" codec=" << m_ifmt_ctx->streams[0]->codecpar->codec_id <<
	" format=" << m_ifmt_ctx->streams[0]->codecpar->format <<
	" width=" << m_ifmt_ctx->streams[0]->codecpar->width <<
	" height=" << m_ifmt_ctx->streams[0]->codecpar->height << std::endl;

	Config::setIntVar(VIDEO_FPS, m_ifmt_ctx->streams[0]->r_frame_rate.num);
	Config::setIntVar(VIDEO_CODEC, m_ifmt_ctx->streams[0]->codecpar->codec_id);
	Config::setIntVar(VIDEO_FORMAT, m_ifmt_ctx->streams[0]->codecpar->format);
	Config::setIntVar(VIDEO_WIDTH, m_ifmt_ctx->streams[0]->codecpar->width);
	Config::setIntVar(VIDEO_HEIGHT, m_ifmt_ctx->streams[0]->codecpar->height);
	
	return 1;
}

} // namespace net