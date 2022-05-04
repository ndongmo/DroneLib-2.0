#include "PCController.h"
#include "Constants.h"

#include <net/NetTcp.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

int PCController::begin() {	
	if (discovery() == -1) {
		logE << "Discovery error" << std::endl;
		return -1;
	}

	m_sender.init(m_droneRcvPort, m_maxFragmentSize, m_maxFragmentNumber);
	if(m_sender.begin() == -1) {
		logE << "Network sender begin failed!" << std::endl;
		return -1;
	}

	m_receiver.init(m_droneSendPort, m_maxFragmentSize, m_maxFragmentNumber);
	if(m_receiver.begin() == -1) {
		logE << "Network receiver begin failed!" << std::endl;
		return -1;
	}

	if(m_window.begin() == -1) {
		logE << "UI begin failed!" << std::endl;
		return -1;
	}

	return 1;
}

int PCController::end() {	
    int result = 0;

	m_running = false;
	
	result += m_sender.end();
	result += m_receiver.end();
	result += m_window.end();

	if(result != 3) return -1;
	else return 1;
}

void PCController::start() {
	m_sender.start();
	m_receiver.start();
	m_window.start();

	run();
}

void PCController::run() {
	m_running = true;

	while (m_running)
	{
		// run window in the main thread
		m_window.run();
		m_running = m_window.isRunning();
	}
	end();
}

int PCController::discovery() {
	net::NetTcp conSocket;
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (conSocket.openClient(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open client error" << std::endl;
        return -1;
	}
	
    nlohmann::json json = {
        {CTRL_PORT_RCV, Config::getInt(CTRL_PORT_RCV, CTRL_PORT_RCV_DEFAULT)}
    };
    std::string msg = json.dump();

	if (conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send PC config failed" << std::endl;
		return -1;
	}

	char buf[1024] = {0};

	if (conSocket.receive(buf, 1024) == -1) {
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

	return 1;
}