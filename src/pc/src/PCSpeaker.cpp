#include "PCSpeaker.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#include <thread>

#define MAX_UPDATE_TRY 100

using namespace utils;

PCSpeaker::PCSpeaker(IStreamListener& stream) : m_stream(stream) {
    m_name = "PCSpeaker";
}

int PCSpeaker::begin() {	
	m_streamLen = 0;
    m_streamPos = nullptr;

    setVolume(Config::getInt(PC_VOLUME));

	return 1;
}

int PCSpeaker::end() {	
	return 1;
}

void PCSpeaker::start() {
    SDL_AudioSpec desired;
	desired.freq = Config::getInt(AUDIO_SAMPLE);
	desired.channels = Config::getInt(AUDIO_CHANNELS);
	desired.samples = Config::getInt(AUDIO_NB_SAMPLES);
    desired.format = getAudioFormat();
	desired.callback = PCSpeaker::forwardCallback;
	desired.userdata = this;
    desired.silence = 0;

    if (SDL_OpenAudio(&desired, &m_params) < 0) {
		logE << "PCSpeaker: couldn't open audio. " << SDL_GetError() << std::endl;
		return;
	}

	m_running = true;
    SDL_PauseAudio(0);

    logI << toString() << std::endl;
}

void PCSpeaker::stop() {
    if(m_running) {
        m_running = false;
        SDL_PauseAudio(1);
        SDL_CloseAudio();
    }
}

void PCSpeaker::run() {

}

void PCSpeaker::audioCallback(UINT8 *data, int len) {
    memset(data, 0, len);
    while(len > 0) {
        if(m_streamLen <= 0 && !tryUpdateStream()) {
            logW << "PCSpeaker: not able to update stream data" << std::endl;
            break;
        }
        const int w = (len > m_streamLen ? m_streamLen : len);
        SDL_MixAudio(data, m_streamPos, w, m_volume);
        m_streamPos += w;
        m_streamLen -= w;
        len -= w;
    }
}

void PCSpeaker::setVolume(int volume) {
    m_volume = volume;
    if(m_volume > SDL_MIX_MAXVOLUME) {
        m_volume = SDL_MIX_MAXVOLUME;
    }
}

void PCSpeaker::updateStream() {
    m_stream.frameHandled();
    m_stream.updateFrame();
    if(m_stream.hasNewFrame()) {
        m_streamLen = m_stream.getSize();
        m_streamPos = m_stream.getData();
    }
}

bool PCSpeaker::tryUpdateStream() {
    unsigned int attempts = 1;

    updateStream();

    while(m_streamLen <= 0 && attempts < MAX_UPDATE_TRY) {
        std::this_thread::sleep_for(std::chrono::microseconds(10 * attempts++));
        updateStream();
    }

    return m_streamLen > 0;
}

SDL_AudioFormat PCSpeaker::getAudioFormat() const {
    if(Config::getInt(AUDIO_FORMAT) == AV_SAMPLE_FMT_U8) {
        return AUDIO_U8;
    } else if(Config::getInt(AUDIO_FORMAT) == AV_SAMPLE_FMT_S16) {
        return AUDIO_S16;
    } else if(Config::getInt(AUDIO_FORMAT) == AV_SAMPLE_FMT_S32) {
        return AUDIO_S32;
    } else if(Config::getInt(AUDIO_FORMAT) == AV_SAMPLE_FMT_FLT) {
        return AUDIO_F32;
    } else {
        return AUDIO_S16SYS;
    }
}

std::string PCSpeaker::toString() {
    if(m_running) {
        return m_name + 
            ": freq=" + std::to_string(m_params.freq) +
			" channels=" + std::to_string(m_params.channels) +
			" samples=" + std::to_string(m_params.samples) +
			" format=" + std::to_string(m_params.format);
    }
    return m_name +": not started";
}