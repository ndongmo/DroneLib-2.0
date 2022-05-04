#include "DroneController.h"
#include "Constants.h"

#include <net/NetTcp.h>
#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

int DroneController::begin() {	
	if (discovery() == -1) {
		logE << "Discovery error" << std::endl;
		return -1;
	}
	
	return 1;
}

int DroneController::end() {
	return 1;
}

void DroneController::start() {
	run();
}

void DroneController::run() {
	
	end();
}

int DroneController::discovery() {
	net::NetTcp conSocket;
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);
	struct sockaddr_in client;

	if (conSocket.openServer(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open server error" << std::endl;
        return -1;
	}

	if (conSocket.listen(client) == -1) {
		logE << "Discovery: TCP listen client error" << std::endl;
        return -1;
	}

	m_clientAddr = net::NetHelper::getIpv4Addr(client);

	char buf[1024] = {0};
	nlohmann::json json;

	if (conSocket.receive(buf, 1024) == -1) {
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

	if (conSocket.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send config failed" << std::endl;
		return -1;
	}

	return 1;
}