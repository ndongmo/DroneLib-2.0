/*!
* \file BatteryController.h
* \brief Battery listener class.
* \author Ndongmo Silatsa
* \date 10-02-2023
* \version 2.0
*/

#pragma once

#include "hardware/I2C.h"
#include "ComponentController.h"

namespace net {
class NetSender;
}

using namespace net;

namespace controller
{

/**
 * Battery controller class in charge of monitoring the battery life.
 */
class BatteryController : public ComponentController<float, 1>
{
public: 
    BatteryController(NetSender& sender);
    
    int begin() override;
    int end() override;

protected:
    void handleActions() override;

    /**
     * @brief Send the battery life percentage value.
     * It is made virtual for testing purposes.
     */
    virtual void sendLifePercentage();

    /** Network sender reference */
	NetSender &m_sender;
    /** PCA9685 instance */
    hardware::I2C m_pca;
};
} // controller