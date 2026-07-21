/*!
* \file PCSpeaker.h
* \brief PC audio output class.
* \author Ndongmo Silatsa
* \date 03-02-2023
* \version 2.0
*/

#pragma once

#include <Service.h>
#include <stream/Stream.h>

#include <SDL3/SDL.h>
#include <vector>
#include <thread>

#ifdef _WIN32 
#include <WS2tcpip.h>
#else
#include <sys/types.h>

typedef uint8_t UINT8;
#endif

using namespace stream;

/**
 * PC audio output service class which acts as a speaker.
 */
class PCSpeaker : public Service
{
public:
    /**
     * @brief Construct a new PCSpeaker object
     * 
     * @param stream stream listener reference
     */
    PCSpeaker(IStreamListener& stream);

    /**
     * @brief Set the Volume
     * 
     * @param volume desired volume <= 128
     */
    void setVolume(int volume);

    int begin() override;
    void start() override;
    void stop() override;
    int end() override;
    void run() override;
    bool isActive() const override;
    std::string toString() override;

private:

    /**
     * @brief Get the Audio Format enum
     * 
     * @return SDL_AudioFormat 
     */
    SDL_AudioFormat getAudioFormat() const;

    /** Current volume */
    float m_volume = 0;
    /** Audio stream listener reference */
    IStreamListener& m_stream;
    /** Managed SDL3 audio stream handle */
    SDL_AudioStream* m_audioStream = nullptr;
    /** Obtained speaker parameters */
    SDL_AudioSpec m_params;
    /** Component handling thread */
    std::thread m_process;
};