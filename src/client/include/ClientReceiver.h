/*!
* \file ClientReceiver.h
* \brief Client receiver service class.
* \author Ndongmo Silatsa
* \date 20-09-2023
* \version 2.0
*/

#pragma once

#include <net/NetReceiver.h>

class ClientSender;
class BatteryReceiver;
namespace stream {
class StreamReceiver;
}

using namespace net;
using namespace stream;

/**
 * Client receiver service class. Handle client -> drone connection and received frames.
 */
class ClientReceiver : public NetReceiver
{
public:
    /**
     * @brief Construct a new Client network receiver object
     * 
     * @param batReceiver Battery receiver reference
     * @param sender network sender reference
     * @param videoReceiver video stream receiver reference
     * @param audioReceiver audio stream receiver reference
     */
    ClientReceiver(BatteryReceiver &batReceiver, ClientSender &sender, 
        StreamReceiver &videoReceiver, StreamReceiver &audioReceiver);

    int begin() override;

private:
    void innerRun(NetFrame &netFrame) override;

    /** Battery receiver reference */
    BatteryReceiver &m_batReceiver;
    /** Client network sender reference */
    ClientSender &m_clientSender;
    /** Video stream receiver reference */
    StreamReceiver &m_videoReceiver;
    /** Audio stream receiver reference */
    StreamReceiver &m_audioReceiver;
};