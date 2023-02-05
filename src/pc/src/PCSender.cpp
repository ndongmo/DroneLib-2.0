#include "PCSender.h"
#include "Constants.h"

#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Constants.h>

PCSender::PCSender() : NetSender() {
	addCommand(NS_ID_NAV, NS_FREQ_NAV);
}

int PCSender::begin() {
	if(NetSender::begin() == -1) {
		return -1;
	}

	int sendPort = Config::getInt(CTRL_PORT_SEND);
	int droneRcvPort = Config::getInt(DRONE_PORT_RCV);
    std::string droneAddr = Config::getString(DRONE_ADDRESS);

	if (m_sendSocket.open(droneAddr.c_str(), droneRcvPort, "", sendPort) == -1) {
		logE << "UDP send socket open error" << std::endl;
        return -1;
	}

	return 1;
}

void PCSender::sendNav(int deltatime, DroneDir dir, DroneSpeed speed) {
	if(canSend(NS_ID_NAV, deltatime)) {
		sendFrame(NS_ID_NAV, NS_FRAME_TYPE_DATA, "12", dir, speed);
		m_currentFreqs[NS_ID_NAV] = 0;
	} else {
		m_currentFreqs[NS_ID_NAV] += deltatime;
	}
}