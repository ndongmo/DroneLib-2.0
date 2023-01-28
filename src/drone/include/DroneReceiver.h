/*!
* \file DroneReceiver.h
* \brief Drone receiver service class.
* \author Ndongmo Silatsa
* \date 09-05-2022
* \version 2.0
*/

#pragma once

#include "DroneSender.h"
#include "controller/MotorController.h"

#include <net/NetReceiver.h>

using namespace net;
using namespace controller;

/**
 * Drone receiver service class which handle network connection and frames sending.
 */
class DroneReceiver : public NetReceiver
{
public:
    /**
     * @brief Construct a new Drone Receiver object
     * 
     * @param sender drone network sender reference
     * @param motorCtrl motor controller reference
     */
    DroneReceiver(DroneSender& sender, MotorController& motorCtrl);

    int begin() override;

private:
    void innerRun(NetFrame& netFrame) override;

    /** Drone network sender reference */
    DroneSender& m_droneSender;
    /** Motor controller reference */
    MotorController& m_motorCtrl;
};