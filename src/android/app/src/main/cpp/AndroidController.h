/*!
* \file AndroidController.h
* \brief Android main controller class.
* \author Ndongmo Silatsa
* \date 26-09-2023
* \version 2.0
*/

#pragma once

#include <utils/Structs.h>
#include <net/NetTcp.h>
#include <stream/AudioReceiver.h>
#include <stream/VideoReceiver.h>
#include <ClientController.h>

#include <jni.h>

/**
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class AndroidController : public ClientController
{
public:
    /**
     * Android constructor.
     */
    AndroidController();

    /**
     * Initialize the config.
     * @param evHandler event handler
     */
    void initConfig(IEventHandler* evHandler);

    /**
     * Get the audio stream buffer
     * 
     * @param buffer the current audio frame
     * @return the size of the returned buffer
     */
    int getAudioStream(const UINT8 **buffer);

    /**
     * Get the video stream buffer
     * 
     * @param buffer the current video frame
     * @return the size of the returned buffer
     */
    int getVideoStream(const UINT8 **buffer);

    /**
     * @return the battery level
     */
    int getBatteryLevel();

    /**
     * @return the current debug message, otherwise empty string.
     */
    std::string getDebugMessage();

private:
    void innerRunServices() override;
    void innerUpdateState(utils::AppState state, int error) override;

    std::string m_msg;
};