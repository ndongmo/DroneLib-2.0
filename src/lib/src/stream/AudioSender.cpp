#include "stream/AudioSender.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"


using namespace utils;

namespace stream {

AudioSender::AudioSender(NetSender &sender) : StreamSender(sender) {
	m_mediaType = AVMEDIA_TYPE_AUDIO;
	m_streamID = NS_ID_STREAM_AUDIO;
}

int AudioSender::begin() {
	m_filename = Config::getStringVar(AUDIO_DEVICE);
	m_formatname = Config::getStringVar(AUDIO_INPUT_FORMAT);
	
	if(StreamSender::begin() == -1) {
		return -1;
	}

	logI << "AudioSender:" <<
	" channels=" << m_ifmt_ctx->streams[0]->codecpar->ch_layout.nb_channels <<
	" bit_rate=" << m_ifmt_ctx->streams[0]->codecpar->bit_rate <<
	" sample_rate=" << m_ifmt_ctx->streams[0]->codecpar->sample_rate <<
	" frame_size=" << m_ifmt_ctx->streams[0]->codecpar->frame_size <<
	" codec=" << m_ifmt_ctx->streams[0]->codecpar->codec_id <<
	" format=" << m_codec->sample_fmts[0] << std::endl;

	Config::setIntVar(AUDIO_CHANNELS, m_ifmt_ctx->streams[0]->codecpar->ch_layout.nb_channels);
	Config::setIntVar(AUDIO_BIT_RATE, m_ifmt_ctx->streams[0]->codecpar->bit_rate);
	Config::setIntVar(AUDIO_SAMPLE, m_ifmt_ctx->streams[0]->codecpar->sample_rate);
	Config::setIntVar(AUDIO_CODEC, m_ifmt_ctx->streams[0]->codecpar->codec_id);
	Config::setIntVar(AUDIO_FORMAT, m_codec->sample_fmts[0]);
	
	return 1;
}

} // namespace net