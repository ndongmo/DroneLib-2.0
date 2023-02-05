/*!
* \file PCReceiver.h
* \brief PC receiver service class.
* \author Ndongmo Silatsa
* \date 09-05-2022
* \version 2.0
*/

#pragma once

#include "PCSender.h"

#include <net/NetReceiver.h>
#include <stream/StreamReceiver.h>

using namespace net;
using namespace stream;

/**
 * PC receiver service class. Handle pc -> drone connection and received frames.
 */
class PCReceiver : public NetReceiver
{
public:
    /**
     * @brief Construct a new PC network receiver object
     * 
     * @param receiver network sender reference
     * @param videoReceiver video stream receiver reference
     * @param audioReceiver audio stream receiver reference
     */
    PCReceiver(PCSender &sender, StreamReceiver &videoReceiver, 
        StreamReceiver &audioReceiver);

    int begin() override;

private:
    void innerRun(NetFrame &netFrame) override;

    /** PC network sender reference */
    PCSender &m_pcSender;
    /** Video stream receiver reference */
    StreamReceiver &m_videoReceiver;
    /** Audio stream receiver reference */
    StreamReceiver &m_audioReceiver;
};