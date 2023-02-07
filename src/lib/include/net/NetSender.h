/*!
* \file NetSender.h
* \brief Network sender service class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <Service.h>
#include <net/NetUdp.h>
#include <net/NetHelper.h>

#include <thread>
#include <mutex>
#include <unordered_map>

namespace net 
{

/**
 * Network frame sender service.
 */
class NetSender : public Service
{
public:
	/**
	 * Construct a new NetSender object
	 */
	NetSender();

	int begin() override;
	void start() override;
	int end() override;

	/**
	 * Sends an ack frame.
	 * \param id frame id
     * \param seq sequence number
	 */
	void sendAck(UINT8 id, UINT8 seq);

    /**
	 * Send a frame with the given data.
	 * \param id frame id
	 * \param type frame type
	 * \param dataSize data size
	 * \param data data to write on the buffer
	 */
	void sendFrame(int id, int type, int dataSize, UINT8* data);

	/**
	 * Decode the given format and send a frame with the contained data.
	 * \param id frame id
	 * \param type frame type
	 * \param format data format
	 */
	void sendFrame(int id, int type, const char* format, ...);

	/**
	 * Send a frame with the given stream fragment.
	 * \param id frame id
	 * \param type frame type
	 * \param fragment stream fragment
	 */
	void sendFrame(int id, int type, const StreamFragment &fragment);

	/**
	 * Sends a ping data.
     * @param deltatime elapsed time
     * @param seq sequence number
	 */
	void sendPing(int deltatime, UINT8 seq);

    /**
	 * Sends a pong data.
     * \param seq sequence number
	 */
	void sendPong(UINT8 seq);

    /**
	 * Sends a quit command.
	 */
	void sendQuit();

	/**
	 * Check if the sender socket is connected.
	 * @return true if the socket is open, false otherwise
	 */
	bool isConnected();

protected:    
	void run() override;
	
	/**
	 * Writes the given frame header parameters on the buffer.
	 * \param id frame id
	 * \param type frame type
	 * \param totalSize frame total size
	 */
	void writeFrameHeader(int id, int type, int totalSize);

    /**
	 * Returns the length of integers in the given string format.
	 * \param format string format
     * \return length
	 */
	int getLength(const char* format);

	/**
	 * Returns the next sequence number at the given buffer id.
	 * \param id buffer id
	 * \param seqBuf buffer's sequence
     * \return next sequence number
	 */
	UINT8 getNextSeqID(int id);

	/**
     * Add the command with the given id for frequency handling.
     * Should only be used for commands which need frequency for sending.
     * 
     * @param id command id
     * @param frequency command frequency
     */
    void addCommand(int id, unsigned int frequency);

    /**
     * Check if the command with the given id can be sent. If it is 
	 * the case, reset the timer for the given id otherwise increment 
	 * this timer. Should only be used for commands which need frequency for sending.
     * 
     * @param id command id
     * @param deltatime elapsed time
     * @return true if the deltatime is bigger than the command frequency,
     * false otherwise
     */
    bool CheckAndUpdateSend(int id, int deltatime);

    /** Buffer's sequence */
	int* m_seqBuf = nullptr;
    /** Buffer */
	UINT8* m_buffer = nullptr;
    /** Mutex watcher for sending */
	std::mutex m_sendMtx;
	/** Defined command frequencies */
    std::unordered_map<int, int> m_freqs;
    /** Current command frequencies */
    std::unordered_map<int, int> m_currentFreqs;
    /** UDP sender socket */
	NetUdp m_sendSocket;
};
} // namespace net