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
     * @brief SDL Audio callback.
     * 
     * @param data audio stream data
     * @param len data length
     */
	void audioCallback(UINT8 *data, int len);

	/**
	 * @brief Forward callback matching SDL3 Audio Stream requirements.
	 * 
	 * @param userdata callback sender context
	 * @param stream the managed SDL audio stream
	 * @param additional_amount bytes needed immediately
	 * @param total_amount total bytes requested
	 */
	static void forwardCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
		PCSpeaker* speaker = static_cast<PCSpeaker*>(userdata);
        
        // Allocate temporary scratch space matching the requested byte size
        std::vector<UINT8> scratchBuffer(additional_amount, 0);
        
        // Execute legacy stream mixing onto the scratch segment
        speaker->audioCallback(scratchBuffer.data(), additional_amount);
        
        // Pipe mixed stream samples cleanly down into the SDL3 stream subsystem
        SDL_PutAudioStreamData(stream, scratchBuffer.data(), additional_amount);
	}


    /**
     * @brief Update stream data length and position.
     */
    void updateStream();

    /**
     * @brief Try to update stream data length and position.
     * 
     * @return true when the stream has been updated, false otherwise.
     */
    bool tryUpdateStream();

    /**
     * @brief Get the Audio Format enum
     * 
     * @return SDL_AudioFormat 
     */
    SDL_AudioFormat getAudioFormat() const;

    /** Current volume */
    float m_volume = 0;
    /** Current stream frame length to read */
    int m_streamLen = 0;
    /** Current stream data position to read */
    const UINT8 *m_streamPos = nullptr;
    /** Audio stream listener reference */
    IStreamListener& m_stream;
    /** Managed SDL3 audio stream handle */
    SDL_AudioStream* m_audioStream = nullptr;
    /** Obtained speaker parameters */
    SDL_AudioSpec m_params;
};