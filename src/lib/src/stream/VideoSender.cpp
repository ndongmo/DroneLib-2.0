#include "stream/VideoSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"


using namespace utils;

namespace stream {

VideoSender::VideoSender(NetSender &sender) : StreamSender(sender) {
	m_name = "VideoCaptureSerive";
	m_mediaType = AVMEDIA_TYPE_VIDEO;
	m_streamID = NS_ID_STREAM_VIDEO;
}

int VideoSender::begin() {
	m_filename = Config::getString(VIDEO_DEVICE);
	m_formatname = Config::getString(VIDEO_INPUT_FORMAT);

	const std::string size = std::to_string(Config::getInt(VIDEO_WIDTH)) + "x" + 
		std::to_string(Config::getInt(VIDEO_HEIGHT));
	const std::string fps = std::to_string(Config::getInt(VIDEO_FPS));

	av_dict_set(&m_options, "video_size", size.c_str(), 0);
  	av_dict_set(&m_options, "r", fps.c_str(), 0);
	
	if(StreamSender::begin() == -1) {
		return -1;
	}

	Config::setInt(VIDEO_FPS, (m_ifmt_ctx->streams[0]->r_frame_rate.num > MAX_FPS ? 
		MAX_FPS : m_ifmt_ctx->streams[0]->r_frame_rate.num));
	Config::setInt(VIDEO_CODEC, m_ifmt_ctx->streams[0]->codecpar->codec_id);
	Config::setInt(VIDEO_FORMAT, m_ifmt_ctx->streams[0]->codecpar->format);
	Config::setInt(VIDEO_WIDTH, m_ifmt_ctx->streams[0]->codecpar->width);
	Config::setInt(VIDEO_HEIGHT, m_ifmt_ctx->streams[0]->codecpar->height);

	logI << toString() << std::endl;
	
	return 1;
}

std::string VideoSender::toString() {
	if(m_ifmt_ctx != NULL && m_ifmt_ctx->nb_streams > 0) {
		return m_name + ": fps=" + std::to_string(Config::getInt(VIDEO_FPS)) +
			" codec=" + std::to_string(Config::getInt(VIDEO_CODEC)) +
			" format=" + std::to_string(Config::getInt(VIDEO_FORMAT)) +
			" width=" + std::to_string(Config::getInt(VIDEO_WIDTH)) +
			" height=" + std::to_string(Config::getInt(VIDEO_HEIGHT));
	}
	return "Not started";
}

} // namespace net