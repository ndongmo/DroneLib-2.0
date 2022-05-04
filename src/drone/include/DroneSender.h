/*!
* \file DroneSender.h
* \brief Drone frame sender class.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#pragma once

#include <net/NetSender.h>

using namespace net;

/*!
 * Drone frame sender service.
 */
class DroneSender : public NetSender
{
public:
	int begin() override;
	void start() override;

    /*!
     * Initialize sender required variables.
     * @param clientRcvPort client reception port
     * @param maxFragmentSize max fragment size
     * @param maxFragmentNumber max number of fragment
     */
    void init(int clientRcvPort, const std::string& clientAddr, 
        int maxFragmentSize, int maxFragmentNumber);
    
	/*!
	 * Sends a ping frame.
     * \param size data size
     * \param data pong data to send
	 */
	void sendPing(int size, UINT8* data);

private:
    void run() override;

    /*! Client reception port */
    int m_clientRcvPort;
    /*! Client address */
    std::string m_clientAddr;
};