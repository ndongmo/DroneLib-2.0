#include "DroneSender.h"
#include "Constants.h"

#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Constants.h>

using namespace utils;

int DroneSender::begin() {
	if(NetSender::begin() == -1) {
		return -1;
	}

	int dronePort = Config::getIntVar(DRONE_PORT_SEND);
	int clienRcvPort = Config::getIntVar(CTRL_PORT_RCV);
    std::string droneAddr = Config::getStringVar(DRONE_ADDRESS);
	std::string clientAddr = Config::getStringVar(CTRL_ADDRESS);

	if (m_sendSocket.open(clientAddr.c_str(), clienRcvPort, droneAddr.c_str(), dronePort) == -1) {
		logE << "UDP send socket open error" << std::endl;
        return -1;
	}

	return 1;
}