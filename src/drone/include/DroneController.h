/*!
* \file DroneController.h
* \brief Drone main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include <Controller.h>

/*!
 * Drone controller service class which starts and stops all others services.
 */
class DroneController : public Controller
{
public:
    int begin() override;
    void start() override;
    int end() override;
    int discovery() override;

    /*!
     * Get the client reception port.
     * \return client reception port
     */
    int getClientRcvPort() const {
        return m_clientRcvPort;
    }

    /*!
     * Get the client address.
     * \return client address
     */
    const std::string& getClientAddr() const {
        return m_clientAddr;
    }
    
private:
    void run() override;

    /*! Client reception port */
    int m_clientRcvPort;
    /*! Client address */
    std::string m_clientAddr;
};