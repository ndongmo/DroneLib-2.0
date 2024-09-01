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
  	av_dict_set(&m_options, "framerate", fps.c_str(), 0);
	
	if(StreamSender::begin() == -1) {
		return -1;
	}

	AVCodecContext *codec_ctx = m_encoder_ctx != NULL ? m_encoder_ctx : m_decoder_ctx;

	Config::setInt(VIDEO_FPS, codec_ctx->framerate.num);
	Config::setInt(VIDEO_WIDTH, codec_ctx->width);
	Config::setInt(VIDEO_HEIGHT, codec_ctx->height);
	Config::setString(VIDEO_PIX_FORMAT, av_get_pix_fmt_name(codec_ctx->pix_fmt));
	Config::setString(VIDEO_ENCODER, avcodec_get_name(codec_ctx->codec_id));

	logI << toString() << std::endl;
	
	return 1;
}

std::string VideoSender::toString() {
	if(m_ifmt_ctx != NULL && m_ifmt_ctx->nb_streams > 0) {
		return m_name + ": fps=" + std::to_string(Config::getInt(VIDEO_FPS)) +
			" codec=" + Config::getString(VIDEO_ENCODER) +
			" format=" + Config::getString(VIDEO_PIX_FORMAT) +
			" width=" + std::to_string(Config::getInt(VIDEO_WIDTH)) +
			" height=" + std::to_string(Config::getInt(VIDEO_HEIGHT));
	}
	return "Not started";
}

void VideoSender::initDecoderCtx() {
	m_decoder_ctx->framerate = av_guess_frame_rate(m_ifmt_ctx, m_istream, NULL);
}

void VideoSender::initEncoderCtx() {
	m_encoder_ctx->height = m_decoder_ctx->height;
	m_encoder_ctx->width = m_decoder_ctx->width;
	m_encoder_ctx->framerate = m_decoder_ctx->framerate;
	m_encoder_ctx->time_base = av_inv_q(m_decoder_ctx->framerate);
	m_encoder_ctx->sample_aspect_ratio = m_decoder_ctx->sample_aspect_ratio;

	if (!Config::getString(VIDEO_PIX_FORMAT).empty()) {
		m_encoder_ctx->pix_fmt = av_get_pix_fmt(Config::getString(VIDEO_PIX_FORMAT).c_str());
	}
	else if (m_encoder->pix_fmts) {
		m_encoder_ctx->pix_fmt = m_encoder->pix_fmts[0];
	}
	else {
		m_encoder_ctx->pix_fmt = m_decoder_ctx->pix_fmt;
	}

	if(m_encoder_ctx->codec_id == AV_CODEC_ID_H264) {
		av_opt_set(m_encoder_ctx->priv_data, "preset", "slow", 0);
	}
	else if (m_encoder_ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
		m_encoder_ctx->max_b_frames = 2;
	}
	else if (m_encoder_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
		m_encoder_ctx->mb_decision = 2;
	}
}

int VideoSender::initFilterCtx() {
	int ret;
	char args[512];
	const AVFilter *buffersrc = avfilter_get_by_name("buffer");
	const AVFilter *buffersink = avfilter_get_by_name("buffersink");

	if (!buffersrc || !buffersink) {
		logE << m_name << ": filtering source or sink element not found" << std::endl;
        return -1;
	}

	snprintf(args, sizeof(args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		m_decoder_ctx->width, m_decoder_ctx->height, m_decoder_ctx->pix_fmt,
		m_decoder_ctx->pkt_timebase.num, m_decoder_ctx->pkt_timebase.den,
		m_decoder_ctx->sample_aspect_ratio.num,
		m_decoder_ctx->sample_aspect_ratio.den);

	ret = avfilter_graph_create_filter(&m_filter_ctx.buffersrc_ctx, buffersrc, "in",
		args, NULL, m_filter_ctx.filter_graph);
	if (ret < 0) {
		logE << m_name << ": failed to create video buffer source -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	ret = avfilter_graph_create_filter(&m_filter_ctx.buffersink_ctx, buffersink, "out",
		NULL, NULL, m_filter_ctx.filter_graph);
	if (ret < 0) {
		logE << m_name << ": failed to create video buffer sink -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	ret = av_opt_set_bin(m_filter_ctx.buffersink_ctx, "pix_fmts",
		(uint8_t*)&m_encoder_ctx->pix_fmt, sizeof(m_encoder_ctx->pix_fmt), AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		logE << m_name << ": failed to set output pixel format -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	return 1;
}

} // namespace net