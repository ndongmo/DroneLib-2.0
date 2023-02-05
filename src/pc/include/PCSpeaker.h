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

#include <SDL/SDL.h>

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
    int end() override;
    void run() override;

private:
    /**
     * @brief SDL Audio callback.
     * 
     * @param data audio stream data
     * @param len data length
     */
	void audioCallback(UINT8 *data, int len);

	/**
	 * @brief Forward callback to simulate SDL audio callback.
	 * 
	 * @param userdata callback sender
	 * @param data audio stream data
	 * @param len data length
	 */
	static void forwardCallback(void *userdata, UINT8 *data, int len) {
		static_cast<PCSpeaker*>(userdata)->audioCallback(data, len);
	}

    /**
     * @brief Get the Audio Format enum
     * 
     * @return SDL_AudioFormat 
     */
    SDL_AudioFormat getAudioFormat() const;

    /** Current volume */
    int m_volume = 0;
    /** Current stream frame len to read */
    int m_streamLen = 0;
    /** Current stream data position to read */
    const UINT8 *m_streamPos = nullptr;
    /** Audio stream listener reference */
    IStreamListener& m_stream;
};