#include "ClientController.h"
#include "IEventHandler.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

ClientController::ClientController() :
	m_evHandler(), m_audioStream(),
	m_receiver(m_batReceiver, m_sender, m_videoStream, m_audioStream) {
	m_name = "ClientController";
}

void ClientController::init() {
	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	innerStopServices();
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

		innerStartServices();
		startService(m_audioStream, MICRO_ACTIVE);
		startService(m_videoStream, CAMERA_ACTIVE);
		
		m_elapsedTime = 1000 / Config::getInt(VIDEO_FPS);

		updateState(APP_RUNNING);
	});
}

void ClientController::initConfigs() {
	Controller::initConfigs();

	Config::setIntDefault(CLIENT_PORT_RCV, CLIENT_PORT_RCV_DEFAULT);
	Config::setIntDefault(CLIENT_PORT_SEND, CLIENT_PORT_SEND_DEFAULT);
}

int ClientController::begin() {	
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

	if(innerBeginServices() == -1) {
		return -1;
	}

	m_sender.setController(this);
	m_receiver.setController(this);
	
	return 1;
}

int ClientController::end() {	
    bool result = true;
	m_running = false;

	m_cv.notify_all();
	m_conSocket.close();

	result = result && m_audioStream.end() != -1;
	result = result && m_videoStream.end() != -1;
	result = result && m_sender.end() != -1;
	result = result && m_receiver.end() != -1;
	result = result && innerEndServices() != -1;

	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	return result ? 1 : -1;
}

void ClientController::start() {
	init();
	run();
}

void ClientController::stop() {
	if(m_sender.isConnected()) {
		m_sender.sendQuit();
	}
	m_running = false;
	m_cv.notify_all();
}

void ClientController::run() {
	m_running = true;
	m_previousClock = std::chrono::steady_clock::now();
	m_elapsedTime = 1000 / Config::getInt(VIDEO_FPS);
	
	while (m_running)
	{
		innerRunServices();
		handleEvents();
		waitNextEvent();
		m_previousClock = std::chrono::steady_clock::now();
	}
}

void ClientController::waitNextEvent() {
	std::unique_lock<std::mutex> lock(m_mutex);
	int tmp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - m_previousClock).count();

	if(tmp < m_elapsedTime) {
		m_cv.wait_for(lock, std::chrono::milliseconds(m_elapsedTime - tmp), [this] { 
			return !m_running;
		});
	}
}

int ClientController::discovery() {
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY);
    std::string serverAddr = Config::getString(DRONE_ADDRESS);

	if (m_conSocket.openClient(serverAddr.c_str(), serverPort) == -1) {
		logE << m_name << " discovery: TCP open client error" << std::endl;
        return -1;
	}

	logI << "fps send: " << Config::getInt(VIDEO_FPS) << std::endl;

	std::string msg = Config::encodeJson({ CLIENT_PORT_RCV, VIDEO_FPS, VIDEO_WIDTH, VIDEO_HEIGHT });

	if (m_conSocket.send(msg.c_str(), (int)msg.length()) == -1) {
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

void ClientController::handleEvents() {
	AppState copyState = m_state;

	if(copyState == APP_CLOSING) {
		m_running = false;
	}
	else if(m_evHandler == nullptr) {
		return;
	}
	else if(copyState != APP_RUNNING) {
		if (m_evHandler->isEventPressed(ClientEvent::QUIT)) {
			updateState(APP_CLOSING);
			m_running = false;
		}
		else if (m_evHandler->isEventPressed(ClientEvent::DISCOVER)) {
			if(copyState == APP_INIT || copyState == APP_ERROR) {
				init();
			}
		}
	}
	else {
		if (m_evHandler->isEventPressed(ClientEvent::QUIT)) {
			updateState(APP_CLOSING);
			m_running = false;
			m_sender.sendQuit();
		}
		else if (m_evHandler->isEventDown(ClientEvent::GO_UP) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(m_elapsedTime, DIR_FORWARD, getSpeed());
		}
		else if (m_evHandler->isEventDown(ClientEvent::GO_DOWN) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(m_elapsedTime, DIR_BACKWARD, getSpeed());
		}
		else if (m_evHandler->isEventDown(ClientEvent::GO_LEFT) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(m_elapsedTime, DIR_LEFT, getSpeed());
		}
		else if (m_evHandler->isEventDown(ClientEvent::GO_RIGHT) && Config::getInt(MOTORS_ACTIVE)) {
			m_sender.sendNav(m_elapsedTime, DIR_RIGHT, getSpeed());
		}
		else if (m_evHandler->isEventDown(ClientEvent::CAM_UP) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(m_elapsedTime, CAMERA_Y_AXE, CAMERA_ROTATION_ANGLE);
		}
		else if (m_evHandler->isEventDown(ClientEvent::CAM_DOWN) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(m_elapsedTime, CAMERA_Y_AXE, -CAMERA_ROTATION_ANGLE);
		}
		else if (m_evHandler->isEventDown(ClientEvent::CAM_LEFT) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(m_elapsedTime, CAMERA_X_AXE, -CAMERA_ROTATION_ANGLE);
		}
		else if (m_evHandler->isEventDown(ClientEvent::CAM_RIGHT) && Config::getInt(SERVOS_ACTIVE)) {
			m_sender.sendCamera(m_elapsedTime, CAMERA_X_AXE, CAMERA_ROTATION_ANGLE);
		}
		else if (m_evHandler->isEventDown(ClientEvent::BUZZ) && Config::getInt(BUZZER_ACTIVE)) {
			m_sender.sendBuzz(m_elapsedTime);
		}
	}

	if(m_oldState != copyState) {
		innerUpdateState(copyState, m_error);
		m_oldState = copyState;
	}
}

DroneSpeed ClientController::getSpeed() {
	DroneSpeed speed = SPEED_LOW;
	if (m_evHandler->isEventDown(ClientEvent::GO_SPEED_1)) {
		speed = SPEED_HIGH;
	} else if (m_evHandler->isEventDown(ClientEvent::GO_SPEED_2)) {
		speed = SPEED_MAX;
	}
	return speed;
}