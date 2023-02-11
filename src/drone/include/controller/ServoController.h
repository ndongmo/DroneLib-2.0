/*!
* \file ServoController.h
* \brief Servo motor controller class.
* \author Ndongmo Silatsa
* \date 05-02-2023
* \version 2.0
*/

#pragma once

#include "component/Motor.h"
#include "hardware/I2C.h"
#include "ComponentController.h"

#include <utils/Structs.h>

using namespace component;
using namespace utils;

namespace controller
{

/**
 * Servo controller class in charge of servo motor rotation.
 */
class ServoController : public ComponentController<ServoAction, SERVO_COUNT>
{
public: 
    ServoController();
    
    int begin() override;
    void start() override;
    int end() override;

    /**
     * Rotate the servo motor for the given axe with the given angle.
     * @param axe rotation axe
     * @param angle rotation angle increment
     */
    void rotate(DroneCamera axe, int angle);

protected:
    void handleActions() override;

    /**
     * @brief Set the angle of all servo to the initial position.
     */
    void init();

    /**
     * @brief Set the Servo pulse with the given angle.
     * 
     * @param index servo index
     * @param angle angle value
     */
    void setServoAngle(unsigned int index, unsigned int angle);

    /** PCA9685 instance */
    hardware::I2C m_pca;
};
} // controller