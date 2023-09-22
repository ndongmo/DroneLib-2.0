/*!
* \file ClientController.h
* \brief Client main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "ClientSender.h"
#include "ClientReceiver.h"
#include "BatteryReceiver.h"
#include "IEventHandler.h"

#include <utils/Structs.h>
#include <net/NetTcp.h>
#include <stream/AudioReceiver.h>
#include <stream/VideoReceiver.h>
#include <Controller.h>

#include <thread>
#include <chrono>
#include <memory>

/**
 * Client controller service class which starts and stops
 * all others services and handle the input events.
 */
class ClientController : public Controller
{
public:
    /**
     * Default constructor
     */
    ClientController();

    int begin() override;
    void start() override;
    void stop() override;
    int end() override;
    int discovery() override;

protected:
    void run() override;
    void initConfigs() override;

    /**
     * Discover, initialize and start sender and receiver components.
     */
    void init();

    /**
     * Handle incoming controller events.
     */
    void handleEvents();

    /** Wait for the next event */
    void waitNextEvent();

    /**
     * Inherited by children class for stopping services.
     */
    virtual void innerStopServices() {}

    /**
     * Inherited by children class for starting services.
     */
    virtual void innerStartServices() {}

    /**
     * Inherited by children class for running services.
     */
    virtual void innerRunServices() = 0;

    /**
     * Inherited by children class for begining services.
     */
    virtual int innerBeginServices() { return 1; }

    /**
     * Inherited by children class for ending services.
     */
    virtual int innerEndServices() { return 1; }

    /**
     * Inherited by children class for Updating the current state.
     * 
     * @param state the new state
     * @param error current error
     */
    virtual void innerUpdateState(utils::AppState state, int error) = 0;

    /**
     * Retrieve the current speed according to the pressed 
     * speed button.
     * @return the current speed
     */
    DroneSpeed getSpeed();

    /** Keep the main process running state */
    bool m_inMainProcess = false;
    /** Elapsed time since the last loop in millisecond */
    int m_elapsedTime = 0;
    /** last recorded clock */
    std::chrono::steady_clock::time_point m_previousClock;

    /** Init process */
	std::thread m_initProcess;
    /** Keyboard/Joystick event handler */
    IEventHandler *m_evHandler = nullptr;
    /** Audio stream receiver object */
    AudioReceiver m_audioStream;
    /** Battery receiver object */
    BatteryReceiver m_batReceiver;
    /** Discovering tcp socket */
    net::NetTcp m_conSocket;
    /** Network sender object */
    ClientSender m_sender;
    /** Network receiver object */
    ClientReceiver m_receiver;
    /** Video stream receiver object */
    VideoReceiver m_videoStream;
};