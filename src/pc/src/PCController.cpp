#include "PCController.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

PCController::PCController() : m_receiver(m_sender), m_window(m_evHandler) {

}

void PCController::init() {
	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

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

		m_sender.init(m_droneRcvPort, m_maxFragmentSize, m_maxFragmentNumber);
		if(m_sender.begin() == -1) {
			logE << "Network sender begin failed!" << std::endl;
			this->handleError(ERROR_NET_INIT);
			return;
		}

		m_receiver.init(m_droneSendPort, m_maxFragmentSize, m_maxFragmentNumber);
		if(m_receiver.begin() == -1) {
			logE << "Network receiver begin failed!" << std::endl;
			this->handleError(ERROR_NET_INIT);
			return;
		}

		m_sender.start();
		m_receiver.start();

		updateState(APP_RUNNING);
	});
}

int PCController::begin() {	
	m_oldState = m_state = APP_INIT;
	m_error = 0;
	m_fps = Config::getInt(CTRL_FPS, CTRL_FPS_DEFAULT);
	
	if(m_window.begin() == -1) {
		logE << "UI begin failed!" << std::endl;
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

	result += m_sender.end();
	result += m_receiver.end();
	result += m_window.end();

	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	if(result != 3) return -1;
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
		unsigned int newTicks = SDL_GetTicks();
		unsigned int elapsedTime = newTicks - m_prevTicks;
		m_prevTicks = newTicks;

		m_window.run();
		handleEvents(elapsedTime);
		m_window.render(elapsedTime);

		if (1000.0f / m_fps > elapsedTime) { // fps limiter
        	SDL_Delay((unsigned int)(1000.0f / (m_fps - elapsedTime)));
		}
	}
	m_inMainProcess = false;
	m_cv.notify_all();
}

int PCController::discovery() {
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_conSocket.openClient(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open client error" << std::endl;
        return -1;
	}
	
    nlohmann::json json = {
        {CTRL_PORT_RCV, Config::getInt(CTRL_PORT_RCV, CTRL_PORT_RCV_DEFAULT)}
    };
    std::string msg = json.dump();

	if (m_conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send PC config failed" << std::endl;
		return -1;
	}

	char buf[1024] = {0};

	if (m_conSocket.receive(buf, 1024) == -1) {
		logE << "Discovery: TCP receive drone config failed" << std::endl;
		return -1;
	}

	try {
        json = nlohmann::json::parse(buf);
		json[DRONE_PORT_RCV].get_to(m_droneRcvPort);
		json[DRONE_PORT_SEND].get_to(m_droneSendPort);
		json[NET_FRAGMENT_SIZE].get_to(m_maxFragmentSize);
        json[NET_FRAGMENT_NUMBER].get_to(m_maxFragmentNumber);
	}
	catch (...) {
		logE << "Json parser error: " << json.dump() << std::endl;
		return -1;
	}

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
		else if (m_evHandler.isEventDown(CtrlEvent::GO_UP)) {
			m_sender.sendNav(elapsedTime, DIR_FORWARD, getSpeed());
		}
		else if (m_evHandler.isEventDown(CtrlEvent::GO_DOWN)) {
			m_sender.sendNav(elapsedTime, DIR_BACKWARD, getSpeed());
		}
		else if (m_evHandler.isEventDown(CtrlEvent::GO_LEFT)) {
			m_sender.sendNav(elapsedTime, DIR_LEFT, getSpeed());
		}
		else if (m_evHandler.isEventDown(CtrlEvent::GO_RIGHT)) {
			m_sender.sendNav(elapsedTime, DIR_RIGHT, getSpeed());
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