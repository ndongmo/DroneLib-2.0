#include "ClientSender.h"

#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Constants.h>

ClientSender::ClientSender() : NetSender() {
	m_name = "ClientSender";
	addCommand(NS_ID_NAV, NS_FREQ_NAV);
	addCommand(NS_ID_CAMERA, NS_FREQ_CAMERA);
}

int ClientSender::begin() {
	if(NetSender::begin() == -1) {
		return -1;
	}

	int sendPort = Config::getInt(CLIENT_PORT_SEND);
	int droneRcvPort = Config::getInt(DRONE_PORT_RCV);
    std::string droneAddr = Config::getString(DRONE_ADDRESS);

	if (m_sendSocket.open(droneAddr.c_str(), droneRcvPort, "", sendPort) == -1) {
		logE << "UDP send socket open error" << std::endl;
        return -1;
	}

	return 1;
}

void ClientSender::sendNav(int deltatime, DroneDir dir, DroneSpeed speed) {
	if(CheckAndUpdateSend(NS_ID_NAV, deltatime)) {
		sendFrame(NS_ID_NAV, NS_FRAME_TYPE_DATA, "12", dir, speed);
	}
}

void ClientSender::sendCamera(int deltatime, DroneCamera axe, int angle) {
	if(CheckAndUpdateSend(NS_ID_CAMERA, deltatime)) {
		bool sign = 1;
		if(angle < 0) { angle = -angle; sign = 0; }
		sendFrame(NS_ID_CAMERA, NS_FRAME_TYPE_DATA, "11b", axe, angle, sign);
	}
}

void ClientSender::sendBuzz(int deltatime) {
	if(CheckAndUpdateSend(NS_ID_BUZZER, deltatime)) {
		sendFrame(NS_ID_BUZZER, NS_FRAME_TYPE_DATA, "");
	}
}