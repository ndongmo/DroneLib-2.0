#include "PCController.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

PCController::PCController() : 
	m_receiver(m_sender, m_videoStream), m_window(m_evHandler) {

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

		updateState(APP_RUNNING);
	});
}

void PCController::initConfigs() {
	int fps = Config::getInt(VIDEO_FPS, VIDEO_FPS_DEFAULT);
	int codec = Config::getInt(VIDEO_CODEC, VIDEO_CODEC_DEFAULT);
	int format = Config::getInt(VIDEO_FORMAT, VIDEO_FORMAT_DEFAULT);
	int width = Config::getInt(VIDEO_WIDTH, VIDEO_WIDTH_DEFAULT);
	int height = Config::getInt(VIDEO_HEIGHT, VIDEO_HEIGHT_DEFAULT);
	int dst_width = Config::getInt(VIDEO_DST_WIDTH, VIDEO_DST_WIDTH_DEFAULT);
	int dst_height = Config::getInt(VIDEO_DST_HEIGHT, VIDEO_DST_HEIGHT_DEFAULT);
	int rcvPort = Config::getInt(CTRL_PORT_RCV, CTRL_PORT_RCV_DEFAULT);
	int sendPort = Config::getInt(CTRL_PORT_SEND, CTRL_PORT_SEND_DEFAULT);
	int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	Config::setIntVar(VIDEO_FPS, fps);
	Config::setIntVar(VIDEO_CODEC, codec);
	Config::setIntVar(VIDEO_FORMAT, format);
	Config::setIntVar(VIDEO_WIDTH, width);
	Config::setIntVar(VIDEO_HEIGHT, height);
	Config::setIntVar(VIDEO_DST_WIDTH, dst_width);
	Config::setIntVar(VIDEO_DST_HEIGHT, dst_height);
	Config::setIntVar(CTRL_PORT_RCV, rcvPort);
	Config::setIntVar(CTRL_PORT_SEND, sendPort);
	Config::setIntVar(DRONE_PORT_DISCOVERY, serverPort);
	Config::setStringVar(DRONE_ADDRESS, serverAddr);
}

int PCController::begin() {	
	m_oldState = m_state = APP_INIT;
	m_error = 0;
	
	initConfigs();
	
	if(m_videoStream.begin() == -1) {
		logE << "Video stream begin failed!" << std::endl;
		return -1;
	}

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

	result += m_videoStream.end();
	result += m_sender.end();
	result += m_receiver.end();
	result += m_window.end();

	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	if(result != 4) return -1;
	else return 1;
}

void PCController::start() {
	m_videoStream.start();
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
			m_window.render(m_videoStream);
			m_videoStream.frameHandled();
		}
		m_fpsLimiter.end();
	}
	m_inMainProcess = false;
	m_cv.notify_all();
}

int PCController::discovery() {
    int serverPort = Config::getIntVar(DRONE_PORT_DISCOVERY);
    std::string serverAddr = Config::getStringVar(DRONE_ADDRESS);

	if (m_conSocket.openClient(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open client error" << std::endl;
        return -1;
	}
	
    nlohmann::json json = {
        {CTRL_PORT_RCV, Config::getIntVar(CTRL_PORT_RCV)}
    };
    std::string msg = json.dump();

	if (m_conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send PC config failed" << std::endl;
		return -1;
	}

	char buf[1024] = {0};
	int droneRcvPort, droneSendPort, maxFragmentSize, maxFragmentNumber,
		fps, width, height, codec, format;

	if (m_conSocket.receive(buf, 1024) == -1) {
		logE << "Discovery: TCP receive drone config failed" << std::endl;
		return -1;
	}

	try {
        json = nlohmann::json::parse(buf);
		json[DRONE_PORT_RCV].get_to(droneRcvPort);
		json[DRONE_PORT_SEND].get_to(droneSendPort);
		json[NET_FRAGMENT_SIZE].get_to(maxFragmentSize);
        json[NET_FRAGMENT_NUMBER].get_to(maxFragmentNumber);
		json[VIDEO_FPS].get_to(fps);
		json[VIDEO_CODEC].get_to(codec);
		json[VIDEO_FORMAT].get_to(format);
		json[VIDEO_WIDTH].get_to(width);
		json[VIDEO_HEIGHT].get_to(height);
	}
	catch (...) {
		logE << "Json parser error: " << json.dump() << std::endl;
		return -1;
	}

	Config::setIntVar(DRONE_PORT_RCV, droneRcvPort);
	Config::setIntVar(DRONE_PORT_SEND, droneSendPort);
	Config::setIntVar(NET_FRAGMENT_SIZE, maxFragmentSize);
	Config::setIntVar(NET_FRAGMENT_NUMBER, maxFragmentNumber);
	Config::setIntVar(VIDEO_FPS, fps);
	Config::setIntVar(VIDEO_CODEC, codec);
	Config::setIntVar(VIDEO_FORMAT, format);
	Config::setIntVar(VIDEO_WIDTH, width);
	Config::setIntVar(VIDEO_HEIGHT, height);

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