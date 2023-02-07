#include "DroneController.h"
#include "Constants.h"

#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>
#include <utils/Structs.h>

using namespace utils;

DroneController::DroneController() : 
	m_receiver(m_sender, m_motorCtrl, m_servoCtrl), m_audioSender(m_sender),
	m_videoSender(m_sender) {
}

void DroneController::init() {
	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	m_audioSender.stop();
	m_videoSender.stop();
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
		m_audioSender.start();
		m_videoSender.start();

		updateState(APP_RUNNING);
	});
}

void DroneController::initConfigs() {
	Controller::initConfigs();
	
	Config::setIntDefault(DRONE_PORT_RCV, DRONE_PORT_RCV_DEFAULT);
	Config::setIntDefault(DRONE_PORT_SEND, DRONE_PORT_SEND_DEFAULT);
	Config::setIntDefault(NET_FRAGMENT_SIZE, NET_FRAGMENT_SIZE_DEFAULT);
	Config::setIntDefault(NET_FRAGMENT_NUMBER, NET_FRAGMENT_NUMBER_DEFAULT);
}

int DroneController::begin() {	
	m_oldState = m_state = APP_INIT;
	
	initConfigs();
	m_sender.setController(this);
	m_receiver.setController(this);

	if(m_ledCtrl.begin() == -1) {
		logE << "DroneController: Leds initialization failed!" << std::endl;
		return -1;
	}
	if(m_motorCtrl.begin() == -1) {
		logE << "DroneController: Motors (Wheels) initialization failed!" << std::endl;
		return -1;
	}
	if(m_servoCtrl.begin() == -1) {
		logE << "DroneController: Servo (Camera motors) initialization failed!" << std::endl;
		return -1;
	}
	if(m_audioSender.begin() == -1) {
		logE << "DroneController: Audio stream capture begin failed!" << std::endl;
		return -1;
	}
	if(m_videoSender.begin() == -1) {
		logE << "DroneController: Video stream capture begin failed!" << std::endl;
		return -1;
	}
	
	return 1;
}

int DroneController::end() {
	int result = 0;

	if(m_sender.isConnected()) {
		m_sender.sendQuit();
	}
	
	m_running = false;

	m_cv.notify_all();
	m_conSocket.close();

	result += m_audioSender.end();
	result += m_videoSender.end();
	result += m_sender.end();
	result += m_receiver.end();
	result += m_ledCtrl.end();
	result += m_motorCtrl.end();
	result += m_servoCtrl.end();

	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	if(result != 7) return -1;
	else return 1;
}

void DroneController::start() {
	m_ledCtrl.start();
	m_motorCtrl.start();
	m_servoCtrl.start();

	init();
	run();
}

void DroneController::stop() {
	m_running = false;
	m_cv.notify_all();
}

void DroneController::run() {
	m_running = true;
	
	while (isRunning())
	{
		handleEvents();
		waitNextEvent();
	}
}

bool DroneController::isRunning() {
	std::lock_guard<std::mutex> guard(m_mutex);
	return m_running;
}

void DroneController::waitNextEvent() {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_oldState = m_state;
	m_cv.wait(lock, [this] { return m_oldState != m_state || !m_running; });
}

void DroneController::handleEvents() {
	if(m_oldState == m_state) return;

	m_ledCtrl.play(m_state);

	if(m_state == APP_INIT || m_state == APP_ERROR) {
		init();
	}
	else if(m_state == APP_CLOSING) {
		m_mutex.lock();
		m_running = false;
		m_mutex.unlock();
		m_cv.notify_all();
	}
}

void DroneController::handleError(int error) {
	if(error > m_error) {
		m_mutex.lock();
		{
			m_error = error;
			m_state = utils::APP_ERROR;
		}
		m_mutex.unlock();
		m_cv.notify_all();
	}
}

int DroneController::discovery() {
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY);
    std::string serverAddr = Config::getString(DRONE_ADDRESS);
	struct sockaddr_in client;

	if (m_conSocket.openServer(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open server error" << std::endl;
        return -1;
	}

	if (m_conSocket.listen(client) == -1) {
		logE << "Discovery: TCP listen client error" << std::endl;
        return -1;
	}

	Config::setString(CTRL_ADDRESS, net::NetHelper::getIpv4Addr(client));

	char buf[1024] = {0};

	if (m_conSocket.receive(buf, 1024) == -1) {
		logE << "Discovery: TCP receive client config failed" << std::endl;
		return -1;
	}

	if(Config::decodeJson(std::string(buf)) == -1) {
		logE << "Discovery: Json parser error" << std::endl;
		return -1;
	}
	logI << "Discovery: receive client address:" <<  Config::getString(CTRL_ADDRESS)
		<< " port:" << Config::getInt(CTRL_PORT_RCV) << std::endl;

	std::string msg = Config::encodeJson({
		DRONE_PORT_RCV, DRONE_PORT_SEND, NET_FRAGMENT_SIZE, NET_FRAGMENT_NUMBER,
		VIDEO_FPS, VIDEO_CODEC, VIDEO_FORMAT, VIDEO_WIDTH, VIDEO_HEIGHT, 
		AUDIO_CODEC, AUDIO_FORMAT, AUDIO_SAMPLE, AUDIO_BIT_RATE, AUDIO_CHANNELS
	});

	if (m_conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send config failed" << std::endl;
		return -1;
	}

	m_conSocket.close();

	return 1;
}