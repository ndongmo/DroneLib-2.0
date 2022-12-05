/*!
* \file DroneController.h
* \brief Drone main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "DroneSender.h"
#include "DroneReceiver.h"
#include "controller/LedController.h"
#include "controller/MotorController.h"

#include <Controller.h>
#include <net/NetTcp.h>

#include <string>
#include <thread>

using namespace controller;

/**
 * Drone controller service class which starts and stops all others services.
 */
class DroneController : public Controller
{
public:
    int begin() override;
    void start() override;
    int end() override;
    int discovery() override;
    bool isRunning() override;
    void handleError(int error) override;

    /**
     * Get the client reception port.
     * \return client reception port
     */
    int getClientRcvPort() const {
        return m_clientRcvPort;
    }

    /**
     * Get the client address.
     * \return client address
     */
    const std::string& getClientAddr() const {
        return m_clientAddr;
    }
    
private:
    void run() override;

    /**
     * Discover, initialize and start sender and receiver components.
     */
    void init();

    /** Handle action in the queue or/and in the list of events */
    void handleEvents();

    /** Wait for the next event */
    void waitNextEvent();

    /** Client reception port */
    int m_clientRcvPort;
    /** Client address */
    std::string m_clientAddr;

    /** Discovering tcp socket */
    net::NetTcp m_conSocket;
    /** Init process */
	std::thread m_initProcess;

    /** Network sender object */
    DroneSender m_sender;
    /** Network receiver object */
    DroneReceiver m_receiver;
    /** Led controller object */
    LedController m_ledCtrl;
    /** Motors (wheels) controller object */
    MotorController m_motorCtrl;
};