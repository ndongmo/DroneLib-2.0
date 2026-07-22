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
    const float amplification = Config::getInt(AUDIO_AMPLIFICATION); 
    const float noiseFloor = Config::getInt(AUDIO_AMPLIFICATION) / 1000.0f; 

    while (m_running) {
        int queuedBytes = SDL_GetAudioStreamQueued(m_audioStream);
        if (queuedBytes >= maxQueuedBytes) {
            SDL_Delay(10);
            continue;
        }

        m_stream.updateFrame();
        if (m_stream.hasNewFrame()) {
            if (amplification > 0) {
                amplifyAudio(m_stream.getData(), m_stream.getSize(), m_params.format, amplification, noiseFloor);
            }
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
            " volume=" + std::to_string((int)(m_volume * MAX_VOLUME)) + 
            " amplification=" + std::to_string(Config::getInt(AUDIO_AMPLIFICATION)) + 
            " noise_floor=" + std::to_string(Config::getInt(AUDIO_NOISE_FLOOR));
    }
    return m_name +": not started";
}

void PCSpeaker::amplifyAudio(UINT8 *buffer, int buffer_size, SDL_AudioFormat format, float target_amp, float noise_floor) {
    // Static envelope trackers across frame boundaries
    static float envelope = 0.0f;
    static float current_gain = 1.0f;
    
    // Hold Time variables to prevent cutting off the end of words
    static int hold_samples_counter = 0;
    const int HOLD_SAMPLES_DURATION = 8820; // Keep gate open for ~200ms after speech ends (at 44.1kHz)

    const float attack_coef = 1.0f;     // Instant open on speech
    
    // SLOWED DOWN: Changed from 0.0005f to 0.0001f for a much smoother fade-out tail
    const float release_coef = 0.0001f; 

    // Determine sample properties based on format
    int sample_size = 1;
    if (format == SDL_AUDIO_S16LE || format == SDL_AUDIO_S16BE) sample_size = sizeof(int16_t);
    else if (format == SDL_AUDIO_F32LE || format == SDL_AUDIO_F32BE) sample_size = sizeof(float);
    else if (format == SDL_AUDIO_S32LE || format == SDL_AUDIO_S32BE) sample_size = sizeof(int32_t);
    
    int num_samples = buffer_size / sample_size;

    for (int i = 0; i < num_samples; i++) {
        float normalized = 0.0f;

        // Extract and normalize current sample based on its format type
        if (format == SDL_AUDIO_S16LE || format == SDL_AUDIO_S16BE) {
            int16_t *ptr = (int16_t *)buffer;
            normalized = ((float)ptr[i]) / 32768.0f;
        } else if (format == SDL_AUDIO_F32LE || format == SDL_AUDIO_F32BE) {
            float *ptr = (float *)buffer;
            normalized = ptr[i];
        } else if (format == SDL_AUDIO_S32LE || format == SDL_AUDIO_S32BE) {
            int32_t *ptr = (int32_t *)buffer;
            normalized = ((float)ptr[i]) / 2147483648.0f;
        } else if (format == SDL_AUDIO_U8) {
            normalized = (((float)buffer[i]) - 128.0f) / 128.0f;
        }

        float abs_val = (normalized < 0.0f) ? -normalized : normalized;

        // 1. Follow the volume envelope
        if (abs_val > envelope) {
            envelope += (abs_val - envelope) * attack_coef;
        } else {
            envelope += (abs_val - envelope) * release_coef;
        }

        // 2. Intelligent Hold State Logic
        float ideal_gain = target_amp;
        if (envelope >= noise_floor) {
            // Speech detected! Reset and hold the gate wide open
            hold_samples_counter = HOLD_SAMPLES_DURATION;
        } else {
            if (hold_samples_counter > 0) {
                // We fell below the noise threshold, but we are inside the hold window
                hold_samples_counter--;
                ideal_gain = target_amp; 
            } else {
                // Hold time expired and it's truly quiet: smoothly apply 15% room attenuation
                ideal_gain = target_amp * 0.15f; 
            }
        }

        // 3. Smooth out gain transitions to prevent digital popping
        current_gain += (ideal_gain - current_gain) * 0.005f;

        // 4. Scale the buffer back out securely
        if (format == SDL_AUDIO_S16LE || format == SDL_AUDIO_S16BE) {
            int16_t *ptr = (int16_t *)buffer;
            float val = ptr[i] * current_gain;
            if (val > 32767.0f)  val = 32767.0f;
            if (val < -32768.0f) val = -32768.0f;
            ptr[i] = (int16_t)val;
        } else if (format == SDL_AUDIO_F32LE || format == SDL_AUDIO_F32BE) {
            float *ptr = (float *)buffer;
            ptr[i] *= current_gain;
            if (ptr[i] > 1.0f)  ptr[i] = 1.0f;
            if (ptr[i] < -1.0f) ptr[i] = -1.0f;
        } else if (format == SDL_AUDIO_S32LE || format == SDL_AUDIO_S32BE) {
            int32_t *ptr = (int32_t *)buffer;
            float val = ptr[i] * current_gain;
            if (val > 2147483647.0f)  val = 2147483647.0f;
            if (val < -2147483648.0f) val = -2147483648.0f;
            ptr[i] = (int32_t)val;
        } else if (format == SDL_AUDIO_U8) {
            float signed_val = (normalized * 128.0f) * current_gain;
            if (signed_val > 127.0f)  signed_val = 127.0f;
            if (signed_val < -128.0f) signed_val = -128.0f;
            buffer[i] = (uint8_t)(signed_val + 128.0f);
        }
    }
}