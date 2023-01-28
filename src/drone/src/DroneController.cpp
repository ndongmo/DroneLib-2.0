#include "DroneController.h"
#include "Constants.h"

#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>
#include <utils/Structs.h>

using namespace utils;

DroneController::DroneController() : 
	m_receiver(m_sender, m_motorCtrl), 
	m_videoSender(NS_ID_STREAM_VIDEO, m_sender) {

}

void DroneController::init() {
	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

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
		m_videoSender.start();

		updateState(APP_RUNNING);
	});
}

void DroneController::initConfigs() {
	int fps = Config::getInt(VIDEO_FPS, VIDEO_FPS_DEFAULT);
	int codec = Config::getInt(VIDEO_CODEC, VIDEO_CODEC_DEFAULT);
	int format = Config::getInt(VIDEO_FORMAT, VIDEO_FORMAT_DEFAULT);
	int width = Config::getInt(VIDEO_WIDTH, VIDEO_WIDTH_DEFAULT);
	int height = Config::getInt(VIDEO_HEIGHT, VIDEO_HEIGHT_DEFAULT);
	int rcvPort = Config::getInt(DRONE_PORT_RCV, DRONE_PORT_RCV_DEFAULT);
	int sendPort = Config::getInt(DRONE_PORT_SEND, DRONE_PORT_SEND_DEFAULT);
	int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
	int maxFragmentSize = Config::getInt(NET_FRAGMENT_SIZE, NET_FRAGMENT_SIZE_DEFAULT);
	int maxFragmentNumber = Config::getInt(NET_FRAGMENT_NUMBER, NET_FRAGMENT_NUMBER_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	Config::setIntVar(VIDEO_FPS, fps);
	Config::setIntVar(VIDEO_CODEC, codec);
	Config::setIntVar(VIDEO_FORMAT, format);
	Config::setIntVar(VIDEO_WIDTH, width);
	Config::setIntVar(VIDEO_HEIGHT, height);
	Config::setIntVar(DRONE_PORT_RCV, rcvPort);
	Config::setIntVar(DRONE_PORT_SEND, sendPort);
	Config::setIntVar(DRONE_PORT_DISCOVERY, serverPort);
	Config::setIntVar(NET_FRAGMENT_SIZE, maxFragmentSize);
	Config::setIntVar(NET_FRAGMENT_NUMBER, maxFragmentNumber);
	Config::setStringVar(DRONE_ADDRESS, serverAddr);
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

	result += m_videoSender.end();
	result += m_sender.end();
	result += m_receiver.end();
	result += m_ledCtrl.end();
	result += m_motorCtrl.end();

	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	if(result != 5) return -1;
	else return 1;
}

void DroneController::start() {
	m_ledCtrl.start();
	m_motorCtrl.start();

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
    int serverPort = Config::getIntVar(DRONE_PORT_DISCOVERY);
    std::string serverAddr = Config::getStringVar(DRONE_ADDRESS);
	struct sockaddr_in client;

	if (m_conSocket.openServer(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open server error" << std::endl;
        return -1;
	}

	if (m_conSocket.listen(client) == -1) {
		logE << "Discovery: TCP listen client error" << std::endl;
        return -1;
	}

	std::string clientAddr = net::NetHelper::getIpv4Addr(client);

	int clientRcvPort;
	char buf[1024] = {0};
	nlohmann::json json;

	if (m_conSocket.receive(buf, 1024) == -1) {
		logE << "Discovery: TCP receive client config failed" << std::endl;
		return -1;
	}

	try {
        json = nlohmann::json::parse(buf);
		json[CTRL_PORT_RCV].get_to(clientRcvPort);
	}
	catch (...) {
		logE << "Discovery: Json parser error" << std::endl;
		return -1;
	}
	
    json = {
        {DRONE_PORT_RCV, Config::getIntVar(DRONE_PORT_RCV)},
		{DRONE_PORT_SEND, Config::getIntVar(DRONE_PORT_SEND)},
		{NET_FRAGMENT_SIZE, Config::getIntVar(NET_FRAGMENT_SIZE)},
		{NET_FRAGMENT_NUMBER, Config::getIntVar(NET_FRAGMENT_NUMBER)},
		{VIDEO_FPS, Config::getIntVar(VIDEO_FPS)},
		{VIDEO_CODEC, Config::getIntVar(VIDEO_CODEC)},
		{VIDEO_FORMAT, Config::getIntVar(VIDEO_FORMAT)},
		{VIDEO_WIDTH, Config::getIntVar(VIDEO_WIDTH)},
		{VIDEO_HEIGHT, Config::getIntVar(VIDEO_HEIGHT)}
    };
    std::string msg = json.dump();

	if (m_conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send config failed" << std::endl;
		return -1;
	}

	Config::setStringVar(CTRL_ADDRESS, clientAddr);
	Config::setIntVar(CTRL_PORT_RCV, clientRcvPort);

	m_conSocket.close();

	return 1;
}