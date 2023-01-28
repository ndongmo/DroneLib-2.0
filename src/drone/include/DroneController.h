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
#include <stream/StreamSender.h>

#include <string>
#include <thread>

using namespace controller;
using namespace stream;

/**
 * Drone controller service class which starts and stops all others services.
 */
class DroneController : public Controller
{
public:
    /**
     * @brief Construct a new Drone Controller object
     * 
     */
    DroneController();

    int begin() override;
    void start() override;
    void stop() override;
    int end() override;
    int discovery() override;
    bool isRunning() override;
    void handleError(int error) override;
    
protected:
    void run() override;

    /**
     * Discover, initialize and start sender and receiver components.
     */
    void init();

    /**
     * Initialize all required config variables.
     */
    void initConfigs();

    /** Handle action in the queue or/and in the list of events */
    void handleEvents();

    /** Wait for the next event */
    void waitNextEvent();

    /** Discovering tcp socket */
    net::NetTcp m_conSocket;
    /** Init process */
	std::thread m_initProcess;

    /** Network sender object */
    DroneSender m_sender;
    /** Network receiver object */
    DroneReceiver m_receiver;
    /** Video stream sender object */
    StreamSender m_videoSender;
    /** Led controller object */
    LedController m_ledCtrl;
    /** Motors (wheels) controller object */
    MotorController m_motorCtrl;
};