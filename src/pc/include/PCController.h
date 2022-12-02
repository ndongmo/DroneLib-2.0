/*!
* \file PCController.h
* \brief PC main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "PCSender.h"
#include "PCReceiver.h"
#include "PCWindow.h"

#include <utils/Structs.h>
#include <net/NetTcp.h>
#include <Controller.h>

#include <thread>

/**
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class PCController : public Controller
{
public:
    int begin() override;
    void start() override;
    int end() override;
    int discovery() override;

    /**
     * Get the drone reception port.
     * \return drone reception port
     */
    int getDroneRcvPort() const {
        return m_droneRcvPort;
    }

    /**
     * Get the drone sending port.
     * \return drone sending port
     */
    int getDroneSendPort() const {
        return m_droneSendPort;
    }

    /**
     * Get the current error.
     * \return error code
     */
    int getError() {
        return m_error;
    }

private:
    void run() override;

    /**
     * Discover, initialize and start sender and receiver components.
     */
    void init();

    /**
     * Handle an incoming command.
     * @param cmd command to handle
     */
    void handleCommands(int cmd);

    /** Current error */
    int m_error = 0;
    /** Discovering tcp socket */
    net::NetTcp m_conSocket;
    /** Init process */
	std::thread m_initProcess;

    /** Drone reception port */
    int m_droneRcvPort;
    /** Drone sending port */
    int m_droneSendPort;

    /** Network sender object */
    PCSender m_sender;
    /** Network receiver object */
    PCReceiver m_receiver;
    /** UI & event manager object */
    PCWindow m_window;
};