#include "DroneSender.h"
#include "Constants.h"

#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Constants.h>

using namespace utils;

void DroneSender::init(int clientRcvPort, const std::string& clientAddr, 
	int maxFragmentSize, int maxFragmentNumber) {
	
	m_clientRcvPort = clientRcvPort;
	m_clientAddr = clientAddr;
	m_maxFragmentSize = maxFragmentSize;
	m_maxFragmentNumber = maxFragmentNumber;
}

int DroneSender::begin() {
	int mySendPort = Config::getInt(DRONE_PORT_SEND, DRONE_PORT_SEND_DEFAULT);
    std::string myAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_sendSocket.open(m_clientAddr.c_str(), m_clientRcvPort, myAddr.c_str(), mySendPort) == -1) {
		logE << "UDP send socket open error" << std::endl;
        return -1;
	}

	m_seqBuf = new int[m_maxFragmentNumber]();
	m_buffer = new UINT8[m_maxFragmentSize]();

	return 1;
}

void DroneSender::start() {
	
}

void DroneSender::run() {
	
}