/*!
* \file DroneReceiver.h
* \brief Drone receiver service class.
* \author Ndongmo Silatsa
* \date 09-05-2022
* \version 2.0
*/

#pragma once

#include <net/NetReceiver.h>

class DroneSender;
namespace controller {
class MotorController;
class ServoController;
class BuzzerController;
}

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
     * @param servoCtrl servo controller reference
     */
    DroneReceiver(DroneSender& sender, MotorController& motorCtrl, 
        ServoController& servoCtrl, BuzzerController& buzzerCtrl);

    int begin() override;

private:
    void innerRun(NetFrame& netFrame) override;

    /** Drone network sender reference */
    DroneSender& m_droneSender;
    /** Motor controller reference */
    MotorController& m_motorCtrl;
    /** Servo controller reference */
    ServoController& m_servoCtrl;
    /** Buzzer controller reference */
    BuzzerController& m_buzzerCtrl;
};