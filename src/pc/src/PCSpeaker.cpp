#include "PCSpeaker.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#define SDL_AUDIO_BUFFER_SIZE 1024

using namespace utils;

PCSpeaker::PCSpeaker(IStreamListener& stream) : m_stream(stream) {

}

int PCSpeaker::begin() {	
	SDL_AudioSpec audio;
	audio.freq = Config::getIntVar(AUDIO_SAMPLE);
	audio.format = getAudioFormat();
	audio.channels = Config::getIntVar(AUDIO_CHANNELS);
	audio.silence = 0;
	audio.samples = SDL_AUDIO_BUFFER_SIZE;
	audio.callback = PCSpeaker::forwardCallback;
	audio.userdata = this;

    m_streamLen = 0;
    m_streamPos = nullptr;

    setVolume(Config::getIntVar(PC_VOLUME));

	if (SDL_OpenAudio(&audio, NULL) < 0) {
		logE << "PCSpeaker: couldn't open audio. " << SDL_GetError() << std::endl;
		return -1;
	}

	return 1;
}

int PCSpeaker::end() {	
    SDL_CloseAudio();

	return 1;
}

void PCSpeaker::start() {
	m_running = true;
    SDL_PauseAudio(0);
}

void PCSpeaker::run() {

}

void PCSpeaker::audioCallback(UINT8 *data, int len) {
	if(m_streamLen <= 0 ) {
        m_stream.frameHandled();
        m_stream.updateFrame();
        if(m_stream.hasNewFrame()) {
            m_streamLen = m_stream.getSize();
            m_streamPos = m_stream.getData();
        }
    }
    else {
        len = (len > m_streamLen ? m_streamLen : len);
        memset(data, 0, len);
        SDL_MixAudio(data, m_streamPos, len, m_volume);
        m_streamPos += len;
        m_streamLen -= len;
    }
}

void PCSpeaker::setVolume(int volume) {
    m_volume = volume;
    if(m_volume > SDL_MIX_MAXVOLUME) {
        m_volume = SDL_MIX_MAXVOLUME;
    }
}

SDL_AudioFormat PCSpeaker::getAudioFormat() const {
    if(Config::getIntVar(AUDIO_FORMAT) == AV_SAMPLE_FMT_U8) {
        return AUDIO_U8;
    } else if(Config::getIntVar(AUDIO_FORMAT) == AV_SAMPLE_FMT_S16) {
        return AUDIO_S16;
    } else if(Config::getIntVar(AUDIO_FORMAT) == AV_SAMPLE_FMT_S32) {
        return AUDIO_S32;
    } else if(Config::getIntVar(AUDIO_FORMAT) == AV_SAMPLE_FMT_FLT) {
        return AUDIO_F32;
    } else {
        return AUDIO_S16SYS;
    }
}