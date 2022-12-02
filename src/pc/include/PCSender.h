/*!
* \file PCSender.h
* \brief PC frame sender class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <net/NetSender.h>

using namespace net;

/**
 * PC frame sender service.
 */
class PCSender : public NetSender
{
public:
	int begin() override;
	void start() override;

    /**
     * Initialize sender required variables.
     * @param droneRcvPort drone reception port
     * @param maxFragmentSize max fragment size
     * @param maxFragmentNumber max number of fragment
     */
    void init(int droneRcvPort, int maxFragmentSize, int maxFragmentNumber);
    
	/**
	 * Sends a pong frame.
     * \param size data size
     * \param data pong data to send
	 */
	void sendPong(int size, UINT8* data);

    /**
     * Get the drone reception port.
     * \return drone reception port
     */
    int getDroneRcvPort() const {
        return m_droneRcvPort;
    }

private:
    void run() override;

    /** Drone reception port */
    int m_droneRcvPort;
};