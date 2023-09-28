#include "AndroidController.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

AndroidController::AndroidController() : ClientController() {
	m_name = "AndroidController";
}

void AndroidController::innerRunServices() {
	if(m_env != nullptr && m_obj != nullptr) {
		/* Java method is: public void update(String msg, int batLife, int fps) */
		jmethodID methodID = m_env->GetMethodID(m_env->GetObjectClass(*m_obj), "update", "(Ljava/lang/String;I;I)V");
		m_env->CallVoidMethod(*m_obj, methodID, m_msg,
			m_batReceiver.getBatteryLife(), Config::getInt(VIDEO_FPS));
	}
}

void AndroidController::innerUpdateState(utils::AppState state, int error) {
	std::string msg;

    if(state == APP_ERROR) {
        msg = logError(error) + " Press [" + 
            m_evHandler->getMapping(ClientEvent::DISCOVER) + "] to restart discovering.";
    }
    else if(state == APP_DISCOVERING) {
        msg = "Drone discovering ongoing on ip [" + 
            Config::getString(DRONE_ADDRESS) + "] and port [" +
            std::to_string(Config::getInt(DRONE_PORT_DISCOVERY)) + "] ...";
    } else {
		msg = "";
	}

	if(m_env != nullptr) {
		m_msg = m_env->NewStringUTF(msg.c_str());
	}
}

int AndroidController::getAudioStream(const UINT8 *buffer) {
	int len = 0;

	m_audioStream.frameHandled();
    m_audioStream.updateFrame();
    if(m_audioStream.hasNewFrame()) {
        len = m_audioStream.getSize();
        buffer = m_audioStream.getData();
    }
	return len;
}

int AndroidController::getVideoStream(const UINT8 *buffer) {
	int len = 0;

	m_videoStream.frameHandled();
    m_videoStream.updateFrame();
    if(m_videoStream.hasNewFrame()) {
        len = m_videoStream.getSize();
        buffer = m_videoStream.getData();
    }
	return len;
}

void AndroidController::updateEnv(JNIEnv* env, jobject obj) {
	m_env = env;
	m_obj = &obj;
}