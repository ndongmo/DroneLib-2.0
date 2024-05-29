#include "AndroidController.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

AndroidController::AndroidController() : ClientController() {
	m_name = "AndroidController";
}

void AndroidController::innerRunServices() {
    // nothing to do yet
}

void AndroidController::innerUpdateState(utils::AppState state, int error) {
    m_msg = "";

    if(state == APP_ERROR) {
        m_msg = logError(error) + " Press [" +
            m_evHandler->getMapping(ClientEvent::DISCOVER) + "] to restart discovering.";
        logE << m_msg << std::endl;
    }
    else if(state == APP_DISCOVERING) {
        m_msg = "Drone discovering ongoing on ip [" +
            Config::getString(DRONE_ADDRESS) + "] and port [" +
            std::to_string(Config::getInt(DRONE_PORT_DISCOVERY)) + "] ...";
        logI << m_msg << std::endl;
    }
}

int AndroidController::getAudioStream(const UINT8 **buffer) {
	int len = 0;

	m_audioStream.frameHandled();
    m_audioStream.updateFrame();
    if(m_audioStream.hasNewFrame()) {
        len = m_audioStream.getSize();
        *buffer = m_audioStream.getData();
    }
	return len;
}

int AndroidController::getVideoStream(const UINT8 **buffer) {
	int len = 0;

	m_videoStream.frameHandled();
    m_videoStream.updateFrame();
    if(m_videoStream.hasNewFrame()) {
        len = m_videoStream.getSize();
        *buffer = m_videoStream.getData();
        logI << "buffer len1:" << len << std::endl;
    }
	return len;
}

void AndroidController::initConfig(IEventHandler* evHandler) {
    m_evHandler = evHandler;
	ClientController::initConfigs();
}

int AndroidController::getBatteryLevel() {
    return m_batReceiver.getBatteryLife();
}

std::string AndroidController::getDebugMessage() {
    return m_msg;
}
