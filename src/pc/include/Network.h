/*!
* \file Network.h
* \brief PC Network service.
* \author Ndongmo Silatsa
* \date 09-05-2022
* \version 2.0
*/

#pragma once

#include <net/NetworkService.h>

using namespace net;

/*!
 * Network service class. Handle network connections for 
 * all PC components.
 */
class Network : public NetworkService
{
public:
    int begin() override;
    int discovery() override;

    /*!
     * Get the drone reception port.
     * \return drone reception port
     */
    int getDroneRcvPort() const {
        return m_droneRcvPort;
    }

private:
    void innerRun(NetworkFrame& NetworkServiceFrame) override;

    /*! Drone reception port */
    int m_droneRcvPort;
};