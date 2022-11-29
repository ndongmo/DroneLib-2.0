/*!
* \file PCController.h
* \brief PC main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include <net/NetTcp.h>
#include <Controller.h>
#include <ErrorListener.h>

#include "PCSender.h"
#include "PCReceiver.h"
#include "PCWindow.h"
#include "PCHelper.h"

/*!
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class PCController : public Controller, public ErrorListener
{
public:
    int begin() override;
    void start() override;
    int end() override;
    int discovery() override;
    void handleError(int error)override;

    /*!
     * Get the drone reception port.
     * \return drone reception port
     */
    int getDroneRcvPort() const {
        return m_droneRcvPort;
    }

    /*!
     * Get the drone sending port.
     * \return drone sending port
     */
    int getDroneSendPort() const {
        return m_droneSendPort;
    }

    /*!
     * Get the current error.
     * \return error code
     */
    int getError() {
        return m_error;
    }

    /*!
     * Get the current state.
     * \return current state
     */
    PCState getState() {
        return m_state;
    }

private:
    void run() override;

    /*!
     * Discover, initialize and start sender and receiver components.
     */
    void init();

    /*!
     * Handle an incoming command.
     * @param cmd command to handle
     */
    void handleCommands(int cmd);

    /*! Current state */
    PCState m_state;
    /*! Current error */
    int m_error = 0;
    /*! Discovering tcp socket */
    net::NetTcp m_conSocket;
    /*! Init process */
	std::thread m_initProcess;

    /*! Drone reception port */
    int m_droneRcvPort;
    /*! Drone sending port */
    int m_droneSendPort;

    /*! Network sender object */
    PCSender m_sender;
    /*! Network receiver object */
    PCReceiver m_receiver;
    /*! UI & event manager object */
    PCWindow m_window;
};