/*!
* \file DroneReceiver.h
* \brief Drone receiver service class.
* \author Ndongmo Silatsa
* \date 09-05-2022
* \version 2.0
*/

#pragma once

#include <net/NetReceiver.h>

using namespace net;

/*!
 * Drone receiver service class which handle network connection and frames sending.
 */
class DroneReceiver : public NetReceiver
{
public:
    int begin() override;

    /*!
     * Initialize receiver required variables.
     * @param clientRcvPort client reception port
     * @param clientAddr client address
     * @param maxFragmentSize max fragment size
     * @param maxFragmentNumber max number of fragment
     */
    void init(int clientRcvPort, const std::string& clientAddr, 
        int maxFragmentSize, int maxFragmentNumber);

private:
    void innerRun(NetFrame& netFrame) override;

    /*! Client reception port */
    int m_clientRcvPort;
    /*! Client address */
    std::string m_clientAddr;
};