/*!
* \file NetworkService.h
* \brief Network communication service class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include "Udp.h"
#include "Tcp.h"
#include "Service.h"
#include "NetworkHelper.h"

#include <thread>

namespace net
{

/*!
 * Network communication service.
 */
class NetworkService : public Service
{
public:
    void start() override;
    int end() override;

	/*!
	 * Starts a handshake connection.
	 * \return -1 when the communication failed, otherwise 1.
	 */
	virtual int discovery() = 0;

	/*!
     * Get the max fragment size.
     * \return max fragment size
     */
    int getMaxFragementSize() const {
        return m_maxFragmentSize;
    }

	/*!
     * Get the max fragment number.
     * \return max fragment number
     */
    int getMaxFragementNumber() const {
        return m_maxFragmentNumber;
    }

protected:
	void run() override;

	/*!
	 * Internal execution of a network service.
	 */
	virtual void innerRun(NetworkFrame& NetworkServiceFrame) = 0;

	/*! Max fragment size */
	int m_maxFragmentSize = 65000;
	/*! Max fragment number */
	int m_maxFragmentNumber = 1;
	/*! Main process */
	std::thread m_process;
	/*! UDP socket writer/reader */
	Udp m_udp;
	/*! TCP socket writer/reader */
	Tcp m_tcp;
};
} // namespace net
