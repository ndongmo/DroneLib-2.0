#include "PCSender.h"
#include "Constants.h"

#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Constants.h>

using namespace utils;

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

void PCSender::sendPong(int size, UINT8* data) {
    sendFrame(NS_MANAGER_INTERNAL_BUFFER_ID_PONG, NS_FRAME_TYPE_DATA, size, data);
}