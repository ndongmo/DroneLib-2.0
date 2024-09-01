/*!
* \file StreamSender.h
* \brief Stream sender service class.
* \author Ndongmo Silatsa Fabrice
* \date 12-12-2022
* \version 2.0
*/

#pragma once

#include "Service.h"
#include "Stream.h"

#include <thread>
#include <mutex>

namespace net {
class NetSender;	
}

using namespace net;

namespace stream
{

/**
 * Generic stream sender service.
 * Send udp stream frames.
 */
class StreamSender: public Service
{
public:
	/**
	 * @brief Construct a new Stream Sender object
	 * 
	 * @param streamID id of the stream
	 * @param sender  network sender reference
	 */
	StreamSender(NetSender &sender);

	int begin() override;
	void start() override;
	void stop() override;
    int end() override;

protected:
    void run() override;

	/**
	 * @brief Send the packet data fragment by fragment through network.
	 * 
	 * @param packet the packet to send
	 */
	virtual void sendPacket(AVPacket *packet);
	/**
	 * @brief Initialize the decoder context.
	 */
	virtual void initDecoderCtx() {}
	/**
	 * @brief Initialize the encoder context.
	 */
	virtual void initEncoderCtx() {}
	/**
	 * @brief Initialize the filter context.
	 * 
	 * @return 1 when the everithing went fine, otherwise -1.
	 */
	virtual int initFilterCtx() { return 1; }
	
	/** Stream ID */
	unsigned int m_streamID;
	/** Frame index */
	unsigned int m_frameIndex = 0;
	/** Captured file name */
	std::string m_filename;
	/** Media input format */
	std::string m_formatname;
	/** Main process */
	std::thread m_process;
	/** Network sender reference */
	NetSender &m_sender;
	/** Type of media (audio/video) */
	AVMediaType m_mediaType;
	/** Media packet */
	AVPacket *m_packet = NULL;
	/** Media frame */
	AVFrame *m_frame = NULL;
	/** Input stream */
	AVStream *m_istream = NULL;
	/** Output stream */
	AVStream *m_ostream = NULL;
	/** Input decoder */
	const AVCodec *m_decoder = NULL;
	/** Input encoder */
	const AVCodec *m_encoder = NULL;
	/** Decoder context */
	AVCodecContext *m_decoder_ctx = NULL;
	/** Encoder context */
	AVCodecContext *m_encoder_ctx = NULL;
	/** Video input options */
	AVDictionary *m_options = NULL;
	/** Input media format context */
	AVFormatContext *m_ifmt_ctx = NULL;
	/** Output media format context */
	AVFormatContext *m_ofmt_ctx = NULL;
	/** Filter context */
	FilteringContext m_filter_ctx;

private:
	/**
	 * @brief Open input stream.
	 * 
	 * @return 1 when the everithing went fine, otherwise -1.
	 */
	int openInputStream();
	/**
	 * @brief Open output stream.
	 * 
	 * @return 1 when the everithing went fine, otherwise -1.
	 */
	int openOutputStream();
	/**
	 * @brief Initialize encoding filters.
	 * 
	 * @return 1 when the everithing went fine, otherwise -1.
	 */
	int initFilters();
	/**
	 * @brief Apply the current filter and encode the received frame.
	 * 
	 * @param frame the frame to filter and encode
	 * @return 1 when the everithing went fine, otherwise -1.
	 */
	int filterEncodeFrame(AVFrame *frame);
	/**
	 * @brief Encode the filtered frame.
	 * 
	 * @param flush should encode NULL for flushing the buffer
	 * @return 1 when the everithing went fine, otherwise -1.
	 */
	int encodeFrame(bool flush);
	/**
	 * @brief Flush decoder, filter and encoder buffers.
	 */
	void flushBuffers();
};
} // namespace stream