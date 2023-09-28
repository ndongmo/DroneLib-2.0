/*!
* \file PCController.h
* \brief PC main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "PCWindow.h"
#include "PCSpeaker.h"

#include <utils/Structs.h>
#include <net/NetTcp.h>
#include <stream/AudioReceiver.h>
#include <stream/VideoReceiver.h>
#include <ClientController.h>

/**
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class PCController : public ClientController
{
public:
    /**
     * Default constructor
     */
    PCController();

protected:
    void initConfigs() override;
    void innerStopServices() override;
    void innerStartServices() override;
    void innerRunServices() override;
    int innerBeginServices() override;
    int innerEndServices() override;
    void innerUpdateState(utils::AppState state, int error) override;

private:
    /** Speaker object */
    PCSpeaker m_speaker;
    /** UI & event manager object */
    PCWindow m_window;
};