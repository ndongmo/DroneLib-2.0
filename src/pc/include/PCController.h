/*!
* \file PCController.h
* \brief PC main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include <Controller.h>
#include "PCSender.h"
#include "PCReceiver.h"
#include "PCWindow.h"

/*!
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class PCController : public Controller
{
public:
    int begin() override;
    void start() override;
    int end() override;
    int discovery() override;

    /*!
     * Get the drone reception port.
     * \return drone reception port
     */
    int getDroneRcvPort() const {
        return m_droneRcvPort;
    }

    /*!
     * Get the drone sending port.
     * \return drone sending port
     */
    int getDroneSendPort() const {
        return m_droneSendPort;
    }

private:
    void run() override;

    /*! Drone reception port */
    int m_droneRcvPort;
    /*! Drone sending port */
    int m_droneSendPort;

    /*! Network sender object */
    PCSender m_sender;
    /*! Network receiver object */
    PCReceiver m_receiver;
    /*! UI & event manager object */
    PCWindow m_window;
};