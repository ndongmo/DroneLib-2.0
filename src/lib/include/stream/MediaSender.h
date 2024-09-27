/*!
* \file MediaSender.h
* \brief Media sender service class.
* \author Ndongmo Silatsa Fabrice
* \date 12-12-2022
* \version 2.0
*/

#pragma once

#include "Service.h"
#include "Stream.h"
#include "VideoSender.h"
#include "AudioSender.h"

#include <mutex>

namespace net {
class NetSender;	
}

using namespace net;

namespace stream
{

class IMediaSender 
{
public:
    /**
     * Write the given packet to the output media file.
     * 
     * @param packet to be written
     * @return 0 on success, otherwise negative value.
     */
    virtual int writePacket(AVPacket *packet) = 0;
};

/**
 * Media sender service.
 * Wrap the video and audio sender in order to synchronize
 * the produced output streams.
 */
class MediaSender: public Service, public IMediaSender
{
public:
	/**
	 * @brief Construct a new Media Sender object
	 * 
	 * @param sender  network sender reference
	 */
	MediaSender(NetSender &sender);

	int begin() override;
	void start() override;
	void stop() override;
    int end() override;
    bool isActive() const override;
    bool isRunning() override;
    int writePacket(AVPacket *packet) override;

    /**
	 * @brief Check whether the stream server connection is cancel.
	 * 
     * @return true when the cancel is requested, otherwise false.
	 */
	bool isCancelRequested() const;

protected:
    void run() override;

    /**
	 * @brief Accept and do the hand-shake for initializing the http connection.
	 * 
     * @return 1 when the everithing went fine, otherwise -1.
	 */
	int startHTTP();

    /**
	 * @brief Abort the current blocking function for opening the media output file.
	 * 
	 * @param arg possible argument
     * @return 1 to abort, otherwise 0
	 */
	static int abortCallback(void *arg);

    /** Mutex for variables synchronization */
    std::mutex m_mutex;
    /** Main process */
	std::thread m_process;
    /** Http client context */
    AVIOContext *m_http_client = NULL;
    /** Output media format context */
	AVFormatContext *m_ofmt_ctx = NULL;
	/** Audio stream sender object */
    AudioSender m_audioSender;
    /** Video stream sender object */
    VideoSender m_videoSender;
};
} // namespace stream