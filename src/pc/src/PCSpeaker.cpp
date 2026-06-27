#include "PCSpeaker.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#include <thread>

#define MAX_UPDATE_TRY 100
#define MAX_VOLUME 256

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
    desired.format = getAudioFormat();

    m_audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 
        &desired, 
        PCSpeaker::forwardCallback, 
        this
    );

    if (!m_audioStream) {
        logE << "PCSpeaker: couldn't open audio device stream. " << SDL_GetError() << std::endl;
        return;
    }

	m_running = true;

    SDL_GetAudioStreamFormat(m_audioStream, &m_params, nullptr);
    SDL_ResumeAudioStreamDevice(m_audioStream);

    logI << toString() << std::endl;
}

void PCSpeaker::stop() {
    if(m_running) {
        m_running = false;
        SDL_PauseAudioStreamDevice(m_audioStream);
        
        SDL_DestroyAudioStream(m_audioStream);
        m_audioStream = nullptr;
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
        SDL_MixAudio(data, m_streamPos, m_params.format, w, m_volume);
        m_streamPos += w;
        m_streamLen -= w;
        len -= w;
    }
}

void PCSpeaker::setVolume(int volume) {
    if(volume > MAX_VOLUME) {
        volume = MAX_VOLUME;
    }
    m_volume = (float)volume / MAX_VOLUME;
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
    AVSampleFormat sample_fmt = (AVSampleFormat)Config::getInt(AUDIO_SAMPLE_FORMAT);

    if(sample_fmt == AV_SAMPLE_FMT_U8) {
        return SDL_AUDIO_U8;
    } else if(sample_fmt == AV_SAMPLE_FMT_S16) {
        return SDL_AUDIO_S16;
    } else if(sample_fmt == AV_SAMPLE_FMT_S32) {
        return SDL_AUDIO_S32;
    } else if(sample_fmt == AV_SAMPLE_FMT_FLT) {
        return SDL_AUDIO_F32;
    } else {
        return SDL_AUDIO_S16;
    }
}

bool PCSpeaker::isActive() const {
    return Config::getInt(MICRO_ACTIVE);
}

std::string PCSpeaker::toString() {
    if(m_running) {
        return m_name + 
            ": freq=" + std::to_string(m_params.freq) +
			" channels=" + std::to_string(m_params.channels) +
			" format=" + std::to_string(m_params.format);
    }
    return m_name +": not started";
}