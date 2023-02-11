/*!
* \file PCReceiver.h
* \brief PC receiver service class.
* \author Ndongmo Silatsa
* \date 09-05-2022
* \version 2.0
*/

#pragma once

#include <net/NetReceiver.h>

class PCSender;
class PCWindow;
namespace stream {
class StreamReceiver;
}

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
     * @param window window reference
     * @param videoReceiver video stream receiver reference
     * @param audioReceiver audio stream receiver reference
     */
    PCReceiver(PCWindow &window, PCSender &sender, 
        StreamReceiver &videoReceiver, StreamReceiver &audioReceiver);

    int begin() override;

private:
    void innerRun(NetFrame &netFrame) override;

    /** PC window reference */
    PCWindow &m_window;
    /** PC network sender reference */
    PCSender &m_pcSender;
    /** Video stream receiver reference */
    StreamReceiver &m_videoReceiver;
    /** Audio stream receiver reference */
    StreamReceiver &m_audioReceiver;
};