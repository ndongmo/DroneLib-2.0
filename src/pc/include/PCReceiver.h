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

using namespace net;

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
     */
    PCReceiver(PCSender& sender);

    int begin() override;

    /**
     * Initialize receiver required variables.
     * @param droneSendPort drone sending port
     * @param maxFragmentSize max fragment size
     * @param maxFragmentNumber max number of fragment
     */
    void init(int droneSendPort, int maxFragmentSize, int maxFragmentNumber);

private:
    void innerRun(NetFrame& netFrame) override;

    /** Drone sending port */
    int m_droneSendPort;
    /** PC network sender reference */
    PCSender& m_pcSender;
};