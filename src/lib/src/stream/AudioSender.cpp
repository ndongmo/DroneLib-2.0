#include "stream/AudioSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

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

	Config::setInt(AUDIO_CHANNELS, m_ifmt_ctx->streams[0]->codecpar->ch_layout.nb_channels);
	Config::setInt(AUDIO_BIT_RATE, (int)m_ifmt_ctx->streams[0]->codecpar->bit_rate);
	Config::setInt(AUDIO_SAMPLE, m_ifmt_ctx->streams[0]->codecpar->sample_rate);
	Config::setInt(AUDIO_CODEC, m_ifmt_ctx->streams[0]->codecpar->codec_id);
	Config::setInt(AUDIO_FORMAT, m_codec->sample_fmts[0]);
	Config::setInt(AUDIO_NB_SAMPLES, getNbSamples());

	logI << toString() << std::endl;
	
	return 1;
}

int AudioSender::getNbSamples() {
	int ret, nb_samples;
	if ((ret = av_read_frame(m_ifmt_ctx, m_packet)) < 0) {
        logE << "StreamSender: cannot read captured frame -> " << av_err2str(ret) << std::endl;
        nb_samples = Config::getInt(AUDIO_NB_SAMPLES);
    } else {
        nb_samples = m_packet->size / m_ifmt_ctx->streams[0]->codecpar->ch_layout.nb_channels / 
			av_get_bytes_per_sample((AVSampleFormat)m_ifmt_ctx->streams[0]->codecpar->format);
        av_packet_unref(m_packet);
    }
	return nb_samples;
}

std::string AudioSender::toString() {
	if(m_ifmt_ctx != NULL && m_ifmt_ctx->nb_streams > 0) {
		return m_name + ": channels=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->ch_layout.nb_channels) +
			" bit_rate=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->bit_rate) +
			" sample_rate=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->sample_rate) +
			" frame_size=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->frame_size) +
			" codec=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->codec_id) +
			" format=" + av_get_sample_fmt_name(m_codec->sample_fmts[0]);
	}
	return m_name +": not started";
}

} // namespace net