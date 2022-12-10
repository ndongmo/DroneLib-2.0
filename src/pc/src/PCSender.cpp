#include "PCSender.h"
#include "Constants.h"

#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Constants.h>

PCSender::PCSender() : NetSender() {
	addCommand(NS_ID_NAV, NS_FREQ_NAV);
}

void PCSender::init(int droneRcvPort, int maxFragmentSize, int maxFragmentNumber) {
	m_droneRcvPort = droneRcvPort;
	m_maxFragmentSize = maxFragmentSize;
	m_maxFragmentNumber = maxFragmentNumber;
}

int PCSender::begin() {
	int mySendPort = Config::getInt(CTRL_PORT_SEND, CTRL_PORT_SEND_DEFAULT);
    std::string droneAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_sendSocket.open(droneAddr.c_str(), m_droneRcvPort, "", mySendPort) == -1) {
		logE << "UDP send socket open error" << std::endl;
        return -1;
	}

	m_seqBuf = new int[m_maxFragmentNumber]();
	m_buffer = new UINT8[m_maxFragmentSize]();

	return 1;
}

void PCSender::start() {
	
}

void PCSender::run() {
	
}

void PCSender::sendQuit() {
	sendFrame(NS_ID_EMPTY, NS_FRAME_TYPE_QUIT, "");
}

void PCSender::sendNav(int deltatime, DroneDir dir, DroneSpeed speed) {
	if(canSend(NS_ID_NAV, deltatime)) {
		sendFrame(NS_ID_NAV, NS_FRAME_TYPE_DATA, "14", dir, speed);
		m_currentFreqs[NS_ID_NAV] = 0;
	} else {
		m_currentFreqs[NS_ID_NAV] += deltatime;
	}
}