#include "PCSpeaker.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#define MAX_VOLUME 256

using namespace utils;

PCSpeaker::PCSpeaker(IStreamListener& stream) : m_stream(stream) {
    m_name = "PCSpeaker";
}

int PCSpeaker::begin() {	
    setVolume(Config::getInt(PC_VOLUME));
	return 1;
}

int PCSpeaker::end() {	
    stop();
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
        NULL, 
        NULL
    );

    if (!m_audioStream) {
        logE << "PCSpeaker: couldn't open audio device stream. " << SDL_GetError() << std::endl;
        return;
    }

    if (!SDL_SetAudioStreamGain(m_audioStream, m_volume)) {
        logW << "PCSpeaker: Failed to set volume: " << SDL_GetError() << std::endl;
    }

    SDL_GetAudioStreamFormat(m_audioStream, &m_params, nullptr);
    SDL_ResumeAudioStreamDevice(m_audioStream);

    m_running = true;
    m_process = std::thread([this]{run(); });
    logI << toString() << std::endl;
}

void PCSpeaker::stop() {
    m_running = false;
    if(m_process.joinable()) {
        m_process.join();
    }

    SDL_PauseAudioStreamDevice(m_audioStream);
    SDL_DestroyAudioStream(m_audioStream);
    m_audioStream = nullptr;
}

void PCSpeaker::run() {
    // A standard 44.1kHz 16-bit mono stream uses 88,200 bytes per second.
    // 8,820 bytes represents roughly 100ms of safe audio cushion.
    const int maxQueuedBytes = 8820; 

    while (m_running) {
        int queuedBytes = SDL_GetAudioStreamQueued(m_audioStream);
        if (queuedBytes >= maxQueuedBytes) {
            SDL_Delay(10);
            continue;
        }

        m_stream.updateFrame();
        if (m_stream.hasNewFrame()) {
            SDL_PutAudioStreamData(m_audioStream, m_stream.getData(), m_stream.getSize());
            m_stream.frameHandled();
        } 
        else {
            SDL_Delay(1); 
        }
    }
}

void PCSpeaker::setVolume(int volume) {
    if(volume > MAX_VOLUME) {
        volume = MAX_VOLUME;
    }
    m_volume = (float)volume / MAX_VOLUME;
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
			" format=" + std::to_string(m_params.format) +
            " volume=" + std::to_string((int)(m_volume * MAX_VOLUME));
    }
    return m_name +": not started";
}