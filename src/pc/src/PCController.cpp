#include "PCController.h"
#include "EventHandler.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

PCController::PCController() : ClientController(), m_speaker(m_audioStream) {
	m_name = "PCController";
	m_evHandler = &m_window.getEventHandler();
}

void PCController::initConfigs() {
	ClientController::initConfigs();

	Config::setIntDefault(PC_VOLUME, PC_VOLUME_DEFAULT);
	Config::setIntDefault(PC_FONT_SIZE, PC_FONT_SIZE_DEFAULT);
	Config::setStringDefault(PC_APP_NAME, PC_APP_NAME_DEFAULT);

	Config::setStringDefault(VIDEO_PIX_FORMAT, PC_VIDEO_PIX_FORMAT_DEFAULT);
	Config::setIntDefault(AUDIO_SAMPLE_FORMAT, PC_AUDIO_SAMPLE_FORMAT_DEFAULT);
}

void PCController::innerStartServices() {
	startService(m_speaker);
	startService(m_window);
}

void PCController::innerStopServices() {
	stopService(m_speaker);
	stopService(m_window);
}

int PCController::innerBeginServices() {	
	if(m_window.begin() == -1) {
		logE << "UI begin failed!" << std::endl;
		return -1;
	}
	if(m_speaker.begin() == -1) {
		logE << "Speaker begin failed!" << std::endl;
		return -1;
	}
	return 1;
}

int PCController::innerEndServices() {	
    bool result = m_speaker.end() == 1;
	result = m_window.end() == 1 && result;
	return result ? 1 : -1;
}

void PCController::innerRunServices() {
	m_window.run();
	m_videoStream.updateFrame();
	m_window.render(m_videoStream, m_batReceiver.getBatteryLife());
	m_videoStream.frameHandled();
}

void PCController::innerUpdateState(utils::AppState state, int error) {
	m_window.updateState(state, error);
}