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
	m_filename = Config::getString(VIDEO_DEVICE);
	
	if(StreamSender::begin() == -1) {
		return -1;
	}

	logI << "VideoSender:" <<
	" fps=" << m_ifmt_ctx->streams[0]->r_frame_rate.num <<
	" codec=" << m_ifmt_ctx->streams[0]->codecpar->codec_id <<
	" format=" << m_ifmt_ctx->streams[0]->codecpar->format <<
	" width=" << m_ifmt_ctx->streams[0]->codecpar->width <<
	" height=" << m_ifmt_ctx->streams[0]->codecpar->height << std::endl;

	Config::setInt(VIDEO_FPS, m_ifmt_ctx->streams[0]->r_frame_rate.num);
	Config::setInt(VIDEO_CODEC, m_ifmt_ctx->streams[0]->codecpar->codec_id);
	Config::setInt(VIDEO_FORMAT, m_ifmt_ctx->streams[0]->codecpar->format);
	Config::setInt(VIDEO_WIDTH, m_ifmt_ctx->streams[0]->codecpar->width);
	Config::setInt(VIDEO_HEIGHT, m_ifmt_ctx->streams[0]->codecpar->height);
	
	return 1;
}

} // namespace net