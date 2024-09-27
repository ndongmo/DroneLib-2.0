/*!
* \file MotorController.h
* \brief Wheel motor controller class.
* \author Ndongmo Silatsa
* \date 21-09-2022
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
 * Motor controller class in charge of wheels manipulation.
 */
class MotorController : public ComponentController<MotorAction, WHEEL_COUNT>
{
public:   
    MotorController();
    
    int begin() override;
    int end() override;
    bool isActive() const override;

    /**
     * Move wheel in the given direction with the given speed.
     * @param dir moving direction
     * @param speed moving speed
     */
    void move(DroneDir dir, DroneSpeed speed);

protected:
    void handleActions() override;

    /** PCA9685 instance */
    hardware::I2C m_pca;
};
} // controller