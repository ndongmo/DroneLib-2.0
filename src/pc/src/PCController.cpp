#include "PCController.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

PCController::PCController() :
	m_speaker(m_audioStream), m_window(m_evHandler), 
	m_receiver(m_window, m_sender, m_videoStream, m_audioStream) {
	m_name = "PCController";
}

void PCController::init() {
	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	stopService(m_speaker, MICRO_ACTIVE);
	stopService(m_audioStream, MICRO_ACTIVE);
	stopService(m_videoStream, CAMERA_ACTIVE);

	m_sender.end();
	m_receiver.end();
	m_conSocket.close();

	m_initProcess = std::thread([this]{ 
		updateState(APP_DISCOVERING);
		
		if (discovery() == -1) {
			m_conSocket.close();
			logE << "Discovery error" << std::endl;
			this->handleError(ERROR_NET_DISCOVERY);
			return;
		}

		if(m_sender.begin() == -1) {
			logE << "Network sender begin failed!" << std::endl;
			this->handleError(ERROR_NET_INIT);
			return;
		}

		if(m_receiver.begin() == -1) {
			logE << "Network receiver begin failed!" << std::endl;
			this->handleError(ERROR_NET_INIT);
			return;
		}

		m_sender.start();
		m_receiver.start();

		startService(m_speaker, MICRO_ACTIVE);
		startService(m_audioStream, MICRO_ACTIVE);
		startService(m_videoStream, CAMERA_ACTIVE);

		updateState(APP_RUNNING);
	});
}

void PCController::initConfigs() {
	Controller::initConfigs();

	Config::setIntDefault(PC_VOLUME, PC_VOLUME_DEFAULT);
	Config::setIntDefault(PC_FONT_SIZE, PC_FONT_SIZE_DEFAULT);
	Config::setStringDefault(PC_APP_NAME, PC_APP_NAME_DEFAULT);

	Config::setIntDefault(VIDEO_DST_WIDTH, VIDEO_DST_WIDTH_DEFAULT);
	Config::setIntDefault(VIDEO_DST_HEIGHT, VIDEO_DST_HEIGHT_DEFAULT);
	Config::setIntDefault(CTRL_PORT_RCV, CTRL_PORT_RCV_DEFAULT);
	Config::setIntDefault(CTRL_PORT_SEND, CTRL_PORT_SEND_DEFAULT);
}

int PCController::begin() {	
	m_oldState = m_state = APP_INIT;
	m_error = 0;
	
	initConfigs();
	
	if(m_videoStream.begin() == -1) {
		logE << "Video stream begin failed!" << std::endl;
		return -1;
	}

	if(m_audioStream.begin() == -1) {
		logE << "Audio stream begin failed!" << std::endl;
		return -1;
	}

	if(m_window.begin() == -1) {
		logE << "UI begin failed!" << std::endl;
		return -1;
	}

	if(m_speaker.begin() == -1) {
		logE << "Speaker begin failed!" << std::endl;
		return -1;
	}

	m_sender.setController(this);
	m_receiver.setController(this);
	
	return 1;
}

int PCController::end() {	
    int result = 0;
	m_running = false;

	{
		// wait the main process to exit and destroy the lock
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, [this] { return !m_inMainProcess; });
	}

	m_conSocket.close();

	result += m_audioStream.end();
	result += m_videoStream.end();
	result += m_sender.end();
	result += m_receiver.end();
	result += m_speaker.end();
	result += m_window.end();

	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	if(result != 6) return -1;
	else return 1;
}

void PCController::start() {
	m_window.start();

	init();
	run();
}

void PCController::stop() {
	if(m_sender.isConnected()) {
		m_sender.sendQuit();
	}
	m_running = false;
	m_inMainProcess = false;
	m_cv.notify_all();
}

void PCController::run() {
	m_running = true;
	m_inMainProcess = true;
	
	while (m_running)
	{
		m_fpsLimiter.begin();
		{
			m_window.run();
			handleEvents(m_fpsLimiter.getFPS());
			m_videoStream.updateFrame();
			m_window.render(m_videoStream, m_fpsLimiter.getFPS());
			m_videoStream.frameHandled();
		}
		m_fpsLimiter.end();
	}
	m_inMainProcess = false;
	m_cv.notify_all();
}

int PCController::discovery() {
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY);
    std::string serverAddr = Config::getString(DRONE_ADDRESS);

	if (m_conSocket.openClient(serverAddr.c_str(), serverPort) == -1) {
		logE << m_name << " discovery: TCP open client error" << std::endl;
        return -1;
	}

	std::string msg = Config::encodeJson({ CTRL_PORT_RCV });

	if (m_conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << m_name << " discovery: TCP send PC config failed" << std::endl;
		return -1;
	}

	char buf[1024] = {0};

	if (m_conSocket.receive(buf, 1024) == -1) {
		logE << m_name << " discovery: TCP receive drone config failed" << std::endl;
		return -1;
	}

	if(Config::decodeJson(std::string(buf)) == -1) {
		logE << m_name << " discovery: Json parser error" << std::endl;
		return -1;
	}
	logI << m_name << " discovery: received from drone -> " <<  std::string(buf) << std::endl;

	m_conSocket.close();

	return 1;
}

void PCController::handleEvents(int elapsedTime) {
	AppState copyState = m_state;

	if(copyState == APP_CLOSING) {
		m_running = false;
	}
	else if(copyState != APP_RUNNING) {
		if (m_evHandler.isEventPressed(CtrlEvent::QUIT)) {
			updateState(APP_CLOSING);
			m_running = false;
		}
		else if (m_evHandler.isEventPressed(CtrlEvent::DISCOVER)) {
			if(copyState == APP_INIT || copyState == APP_ERROR) {
				init();
			}
		}
	}
	else {
		if (m_evHandler.isEventPressed(CtrlEvent::QUIT)) {
			updateState(APP_CLOSING);
			m_running = false;
			m_sender.sendQuit();
		}
		else if (m_evHandler.isEventDown(CtrlEvent::GO_UP) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(elapsedTime, DIR_FORWARD, getSpeed());
		}
		else if (m_evHandler.isEventDown(CtrlEvent::GO_DOWN) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(elapsedTime, DIR_BACKWARD, getSpeed());
		}
		else if (m_evHandler.isEventDown(CtrlEvent::GO_LEFT) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(elapsedTime, DIR_LEFT, getSpeed());
		}
		else if (m_evHandler.isEventDown(CtrlEvent::GO_RIGHT) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(elapsedTime, DIR_RIGHT, getSpeed());
		}
		else if (m_evHandler.isEventDown(CtrlEvent::CAM_UP) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(elapsedTime, CAMERA_Y_AXE, CAMERA_ROTATION_ANGLE);
		}
		else if (m_evHandler.isEventDown(CtrlEvent::CAM_DOWN) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(elapsedTime, CAMERA_Y_AXE, -CAMERA_ROTATION_ANGLE);
		}
		else if (m_evHandler.isEventDown(CtrlEvent::CAM_LEFT) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(elapsedTime, CAMERA_X_AXE, -CAMERA_ROTATION_ANGLE);
		}
		else if (m_evHandler.isEventDown(CtrlEvent::CAM_RIGHT) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(elapsedTime, CAMERA_X_AXE, CAMERA_ROTATION_ANGLE);
		}
	}

	if(m_oldState != copyState) {
		m_window.updateState(copyState, m_error);
		m_oldState = copyState;
	}
}

DroneSpeed PCController::getSpeed() {
	DroneSpeed speed = SPEED_LOW;
	if (m_evHandler.isEventDown(CtrlEvent::GO_SPEED_1)) {
		speed = SPEED_HIGH;
	} else if (m_evHandler.isEventDown(CtrlEvent::GO_SPEED_2)) {
		speed = SPEED_MAX;
	}
	return speed;
}