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
    void innerRun(NetworkFrame& NetworkServiceFrame) override;

    /*! Client reception port */
    int m_clientRcvPort;
    /*! Client address */
    std::string m_clientAddr;
};