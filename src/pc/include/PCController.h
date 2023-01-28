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
#include "EventHandler.h"
#include "FPSLimiter.h"

#include <utils/Structs.h>
#include <net/NetTcp.h>
#include <stream/StreamReceiver.h>
#include <Controller.h>

#include <thread>

/**
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class PCController : public Controller
{
public:
    /**
     * Default constructor
     */
    PCController();

    int begin() override;
    void start() override;
    void stop() override;
    int end() override;
    int discovery() override;

private:
    void run() override;

    /**
     * Discover, initialize and start sender and receiver components.
     */
    void init();

    /**
     * Initialize all required config variables.
     */
    void initConfigs();

    /**
     * Handle incoming controller events.
     * @param elapsedTime time since the last call
     */
    void handleEvents(int elapsedTime);

    /**
     * Retrieve the current speed according to the pressed 
     * speed button.
     * @return the current speed
     */
    DroneSpeed getSpeed();

    /** Keep the main process running state */
    bool m_inMainProcess = false;

    /** Init process */
	std::thread m_initProcess;
    /** Discovering tcp socket */
    net::NetTcp m_conSocket;
    /** Network sender object */
    PCSender m_sender;
    /** Network receiver object */
    PCReceiver m_receiver;
    /** Keyboard/Joystick event handler */
    EventHandler m_evHandler;
    /** FPS limiter object */
    FPSLimiter m_fpsLimiter;
    /** UI & event manager object */
    PCWindow m_window;
    /** Video stream receiver object */
    StreamReceiver m_videoStream;
};