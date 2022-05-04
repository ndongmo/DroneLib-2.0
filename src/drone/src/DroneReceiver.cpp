#include "DroneReceiver.h"
#include "Constants.h"

#include <net/NetTcp.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils; 

void DroneReceiver::init(int clientRcvPort, const std::string& clientAddr, 
	int maxFragmentSize, int maxFragmentNumber) {
	
	m_clientRcvPort = clientRcvPort;
	m_clientAddr = clientAddr;
	m_maxFragmentSize = maxFragmentSize;
	m_maxFragmentNumber = maxFragmentNumber;
}

int DroneReceiver::begin() {
    int myRcvPort = Config::getInt(DRONE_PORT_RCV, DRONE_PORT_RCV_DEFAULT);
    std::string myAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_rcvSocket.open(m_clientAddr.c_str(), m_clientRcvPort, myAddr.c_str(), myRcvPort) == -1) {
		logE << "UDP open error" << std::endl;
        return -1;
	}

	return 1;
}

void DroneReceiver::innerRun(NetFrame& netFrame) {

}