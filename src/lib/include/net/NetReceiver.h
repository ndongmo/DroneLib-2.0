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

#include <thread>

namespace net
{

/*!
 * Network frame receiver service.
 * Listen and handle incoming udp frames.
 */
class NetReceiver : public Service
{
public:
    void start() override;
    int end() override;

protected:
	void run() override;

	/*!
	 * Internal execution of a network service.
	 * \param netFrame received frame
	 */
	virtual void innerRun(NetFrame& netFrame) = 0;

	/* Closing state */
	bool is_closing = false;
	/*! Max fragment size */
	int m_maxFragmentSize;
	/*! Max fragment number */
	int m_maxFragmentNumber;
	/*! Main process */
	std::thread m_process;
	/*! UDP reception socket */
	NetUdp m_rcvSocket;
};
} // namespace net
