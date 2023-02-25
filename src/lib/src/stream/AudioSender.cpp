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
	
	if(StreamSender::begin() == -1) {
		return -1;
	}

	Config::setInt(AUDIO_CHANNELS, m_ifmt_ctx->streams[0]->codecpar->ch_layout.nb_channels);
	Config::setInt(AUDIO_BIT_RATE, (int)m_ifmt_ctx->streams[0]->codecpar->bit_rate);
	Config::setInt(AUDIO_SAMPLE, m_ifmt_ctx->streams[0]->codecpar->sample_rate);
	Config::setInt(AUDIO_CODEC, m_ifmt_ctx->streams[0]->codecpar->codec_id);
	Config::setInt(AUDIO_FORMAT, m_codec->sample_fmts[0]);

	logI << toString() << std::endl;
	
	return 1;
}

std::string AudioSender::toString() {
	if(m_ifmt_ctx != NULL && m_ifmt_ctx->nb_streams > 0) {
		return m_name + ": channels=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->ch_layout.nb_channels) +
			" bit_rate=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->bit_rate) +
			" sample_rate=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->sample_rate) +
			" frame_size=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->frame_size) +
			" codec=" + std::to_string(m_ifmt_ctx->streams[0]->codecpar->codec_id) +
			" format=" + std::to_string(m_codec->sample_fmts[0]);
	}
	return "Not started";
}

} // namespace net