/*!
* \file NetReceiver.h
* \brief Network receiver service class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "NetUdp.h"
#include "Service.h"
#include "NetHelper.h"
#include "NetSender.h"

#include <thread>

namespace net
{

/**
 * Network frame receiver service.
 * Listen and handle incoming udp frames.
 */
class NetReceiver : public Service
{
public:
	/**
	 * @brief Construct a new Net Receiver object
	 * 
	 * @param sender network sender reference
	 */
	NetReceiver(NetSender& sender);

    void start() override;
    int end() override;

	/**
	 * Check if the sender socket is connected.
	 * @return true if the socket is open, false otherwise
	 */
	bool isConnected();

protected:
	void run() override;

	/**
	 * Internal execution of a network service.
	 * \param netFrame received frame
	 */
	virtual void innerRun(NetFrame& netFrame) = 0;

	/* Closing state */
	bool is_closing = false;
	/** Main process */
	std::thread m_process;
	/** UDP reception socket */
	NetUdp m_rcvSocket;
	/** Network sender reference */
	NetSender& m_sender;
};
} // namespace net
