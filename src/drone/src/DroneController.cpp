#include "DroneController.h"
#include "Constants.h"

#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>
#include <utils/Structs.h>

using namespace utils;

void DroneController::init() {
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

		m_sender.init(m_clientRcvPort, m_clientAddr, m_maxFragmentSize, m_maxFragmentNumber);
		if(m_sender.begin() == -1) {
			logE << "Network sender begin failed!" << std::endl;
			this->handleError(ERROR_NET_INIT);
			return;
		}

		m_receiver.init(m_clientRcvPort, m_clientAddr, m_maxFragmentSize, m_maxFragmentNumber);
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

int DroneController::begin() {	
	m_oldState = m_state = APP_INIT;
	
	m_sender.setController(this);
	m_receiver.setController(this);

	if(m_ledCtrl.begin() == -1) {
		logE << "Leds initialization failed!" << std::endl;
		return -1;
	}
	if(m_motorCtrl.begin() == -1) {
		logE << "Motors (Wheels) initialization failed!" << std::endl;
		return -1;
	}
	
	return 1;
}

int DroneController::end() {
	int result = 0;

	m_running = false;

	m_cv.notify_all();
	m_conSocket.close();

	result += m_sender.end();
	result += m_receiver.end();
	result += m_ledCtrl.end();
	result += m_motorCtrl.end();

	if(m_initProcess.joinable()) {
		m_initProcess.join();
	}

	if(result != 4) return -1;
	else return 1;
}

void DroneController::start() {
	m_ledCtrl.start();
	m_motorCtrl.start();

	init();
	run();
}

void DroneController::run() {
	m_running = true;
	
	while (isRunning())
	{
		handleEvents();
		waitNextEvent();
	}
	end();
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
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);
	struct sockaddr_in client;

	if (m_conSocket.openServer(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open server error" << std::endl;
        return -1;
	}

	if (m_conSocket.listen(client) == -1) {
		logE << "Discovery: TCP listen client error" << std::endl;
        return -1;
	}

	m_clientAddr = net::NetHelper::getIpv4Addr(client);

	char buf[1024] = {0};
	nlohmann::json json;

	if (m_conSocket.receive(buf, 1024) == -1) {
		logE << "Discovery: TCP receive client config failed" << std::endl;
		return -1;
	}

	try {
        json = nlohmann::json::parse(buf);
		json[CTRL_PORT_RCV].get_to(m_clientRcvPort);
	}
	catch (...) {
		logE << "Discovery: Json parser error" << std::endl;
		return -1;
	}

	m_maxFragmentSize = Config::getInt(NET_FRAGMENT_SIZE, NET_FRAGMENT_SIZE_DEFAULT);
	m_maxFragmentNumber = Config::getInt(NET_FRAGMENT_NUMBER, NET_FRAGMENT_NUMBER_DEFAULT);
	
    json = {
        {DRONE_PORT_RCV, Config::getInt(DRONE_PORT_RCV, DRONE_PORT_RCV_DEFAULT)},
		{DRONE_PORT_SEND, Config::getInt(DRONE_PORT_SEND, DRONE_PORT_SEND_DEFAULT)},
		{NET_FRAGMENT_SIZE, m_maxFragmentSize},
		{NET_FRAGMENT_NUMBER, m_maxFragmentNumber}
    };
    std::string msg = json.dump();

	if (m_conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send config failed" << std::endl;
		return -1;
	}

	m_conSocket.close();

	return 1;
}