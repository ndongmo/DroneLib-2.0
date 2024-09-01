#include "stream/AudioSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

#include <algorithm>

using namespace utils;

namespace stream {

AudioSender::AudioSender(NetSender &sender) : StreamSender(sender) {
	m_name = "AudioCaptureSerive";
	m_mediaType = AVMEDIA_TYPE_AUDIO;
	m_streamID = NS_ID_STREAM_AUDIO;
}

int AudioSender::begin() {
	m_filename = Config::getString(AUDIO_DEVICE);
	m_formatname = Config::getString(AUDIO_INPUT_FORMAT);

	const std::string channels = std::to_string(Config::getInt(AUDIO_CHANNELS));
	const std::string sample_rate = std::to_string(Config::getInt(AUDIO_SAMPLE));

	av_dict_set(&m_options, "channels", channels.c_str(), 0);
  	av_dict_set(&m_options, "sample_rate", sample_rate.c_str(), 0);
	
	if(StreamSender::begin() == -1) {
		return -1;
	}

	AVCodecContext *codec_ctx = m_encoder_ctx != NULL ? m_encoder_ctx : m_decoder_ctx;

	Config::setInt(AUDIO_CHANNELS, codec_ctx->ch_layout.nb_channels);
	Config::setInt(AUDIO_BIT_RATE, (int)codec_ctx->bit_rate);
	Config::setInt(AUDIO_SAMPLE, codec_ctx->sample_rate);
	Config::setInt(AUDIO_SAMPLE_FORMAT, codec_ctx->sample_fmt);
	Config::setInt(AUDIO_NB_SAMPLES, getNbSamples(codec_ctx->ch_layout.nb_channels, codec_ctx->sample_fmt));
	Config::setString(AUDIO_ENCODER, avcodec_get_name(codec_ctx->codec_id));

	logI << toString() << std::endl;
	
	return 1;
}

int AudioSender::getNbSamples(int nb_channels, int format) {
	int ret, nb_samples;
	if ((ret = av_read_frame(m_ifmt_ctx, m_packet)) < 0) {
        logE << m_name << ": cannot read captured frame -> " << av_err2str(ret) << std::endl;
        nb_samples = Config::getInt(AUDIO_NB_SAMPLES);
    } else {
        nb_samples = m_packet->size / nb_channels / av_get_bytes_per_sample((AVSampleFormat)format);
        av_packet_unref(m_packet);
    }
	return nb_samples;
}

std::string AudioSender::toString() {
	if(m_ifmt_ctx != NULL && m_ifmt_ctx->nb_streams > 0) {
		return m_name + ": channels=" + std::to_string(Config::getInt(AUDIO_CHANNELS)) +
			" bit_rate=" + std::to_string(Config::getInt(AUDIO_BIT_RATE)) +
			" sample_rate=" + std::to_string(Config::getInt(AUDIO_SAMPLE)) +
			" nb_samples=" + std::to_string(Config::getInt(AUDIO_NB_SAMPLES)) +
			" codec=" + Config::getString(AUDIO_ENCODER) +
			" format=" + av_get_sample_fmt_name((AVSampleFormat)Config::getInt(AUDIO_SAMPLE_FORMAT));
	}
	return m_name +": not started";
}

void AudioSender::initEncoderCtx() {
	if (Config::getInt(AUDIO_SAMPLE) > 0) {
		m_encoder_ctx->sample_rate = Config::getInt(AUDIO_SAMPLE);
	} else if (m_encoder->supported_samplerates) {
		m_encoder_ctx->sample_rate = m_encoder->supported_samplerates[0];
	} else {
		m_encoder_ctx->sample_rate = m_decoder_ctx->sample_rate;
	}

	if (Config::getInt(AUDIO_SAMPLE_FORMAT) > 0) {
		m_encoder_ctx->sample_fmt = (AVSampleFormat) Config::getInt(AUDIO_SAMPLE_FORMAT);
	} else if (m_encoder->sample_fmts) {
		m_encoder_ctx->sample_fmt = m_encoder->sample_fmts[0];
	} else {
		m_encoder_ctx->sample_fmt = m_decoder_ctx->sample_fmt;
	}
	
	m_encoder_ctx->time_base = (AVRational){1, m_encoder_ctx->sample_rate};

	if(m_encoder->ch_layouts) {
		const AVChannelLayout *p = m_encoder->ch_layouts;
		const AVChannelLayout *best_ch_layout = p;
    	int best_nb_channels   = 0;
		while (p->nb_channels) {
			if (p->nb_channels > best_nb_channels) {
				best_nb_channels = p->nb_channels;
				best_ch_layout   = p;
			}
			p++;
		}
		av_channel_layout_copy(&m_encoder_ctx->ch_layout, best_ch_layout);
	} else if (Config::getInt(AUDIO_CHANNELS) > 1) {
		const AVChannelLayout chl = (AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO;
		av_channel_layout_copy(&m_encoder_ctx->ch_layout, &chl);
	} else {
		const AVChannelLayout chl = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
		av_channel_layout_copy(&m_encoder_ctx->ch_layout, &chl);
	}
}

int AudioSender::initFilterCtx() {
	int ret;
	char buf[64];
	char args[512];
	const AVFilter *buffersrc = avfilter_get_by_name("abuffer");
	const AVFilter *buffersink = avfilter_get_by_name("abuffersink");

	if (!buffersrc || !buffersink) {
		logE << m_name << ": filtering source or sink element not found" << std::endl;
        return -1;
	}

	if (m_decoder_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC) {
		av_channel_layout_default(&m_decoder_ctx->ch_layout, m_decoder_ctx->ch_layout.nb_channels);
	}
	av_channel_layout_describe(&m_decoder_ctx->ch_layout, buf, sizeof(buf));

	snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%s",
		m_decoder_ctx->pkt_timebase.num, m_decoder_ctx->pkt_timebase.den, m_decoder_ctx->sample_rate,
		av_get_sample_fmt_name(m_decoder_ctx->sample_fmt), buf);
	
	ret = avfilter_graph_create_filter(&m_filter_ctx.buffersrc_ctx, buffersrc, "in",
		args, NULL, m_filter_ctx.filter_graph);
	if (ret < 0) {
		logE << m_name << ": failed to create audio buffer source -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	ret = avfilter_graph_create_filter(&m_filter_ctx.buffersink_ctx, buffersink, "out",
		NULL, NULL, m_filter_ctx.filter_graph);
	if (ret < 0) {
		logE << m_name << ": failed to create video buffer sink -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	ret = av_opt_set_bin(m_filter_ctx.buffersink_ctx, "sample_fmts",
		(uint8_t*)&m_encoder_ctx->sample_fmt, sizeof(m_encoder_ctx->sample_fmt), AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		logE << m_name << ": failed to set output sample format -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	av_channel_layout_describe(&m_encoder_ctx->ch_layout, buf, sizeof(buf));
	ret = av_opt_set(m_filter_ctx.buffersink_ctx, "ch_layouts", buf, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		logE << m_name << ": failed to set output channel layout -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	ret = av_opt_set_bin(m_filter_ctx.buffersink_ctx, "sample_rates",
		(uint8_t*)&m_encoder_ctx->sample_rate, sizeof(m_encoder_ctx->sample_rate), AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		logE << m_name << ": failed to set output sample rate -> " << av_err2str(ret) << std::endl;
		return -1;
	}

	return 1;
}

} // namespace net