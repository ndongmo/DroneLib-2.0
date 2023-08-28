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
#include "controller/ServoController.h"
#include "controller/BuzzerController.h"
#include "controller/BatteryController.h"

#include <Controller.h>
#include <net/NetTcp.h>
#include <stream/VideoSender.h>
#include <stream/AudioSender.h>

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
    void initConfigs() override;
    
    /**
     * Discover, initialize and start sender and receiver components.
     */
    void init();

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
    /** Audio stream sender object */
    AudioSender m_audioSender;
    /** Video stream sender object */
    VideoSender m_videoSender;
    /** Led controller object */
    LedController m_ledCtrl;
    /** Motors (wheels) controller object */
    MotorController m_motorCtrl;
    /** Servo (camera motors) controller object */
    ServoController m_servoCtrl;
    /** Buzzer controller object */
    BuzzerController m_buzzerCtrl;
    /** Battery controller object */
    BatteryController m_batteryCtrl;
};