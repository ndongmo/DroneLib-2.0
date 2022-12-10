/*!
* \file PCSender.h
* \brief PC frame sender class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <net/NetSender.h>
#include <utils/Structs.h>

using namespace net;
using namespace utils;

/**
 * PC frame sender service.
 */
class PCSender : public NetSender
{
public:
    /**
     * Construct a new PCSender object
     */
    PCSender();

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
     * Get the drone reception port.
     * \return drone reception port
     */
    int getDroneRcvPort() const {
        return m_droneRcvPort;
    }

    /**
	 * Sends a quit command.
	 */
	void sendQuit();

    /**
	 * Sends navigation data.
     * @param dir drone direction
     * @param speed drone speed 
	 */
	void sendNav(int deltatime, DroneDir dir, DroneSpeed speed);

private:
    void run() override;

    /** Drone reception port */
    int m_droneRcvPort;
};