#include "PCReceiver.h"
#include "Constants.h"

#include <net/NetTcp.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils; 

void PCReceiver::init(int droneSendPort, int maxFragmentSize, int maxFragmentNumber) {
	m_droneSendPort = droneSendPort;
	m_maxFragmentSize = maxFragmentSize;
	m_maxFragmentNumber = maxFragmentNumber;
}

int PCReceiver::begin() {
    int myRcvPort = Config::getInt(CTRL_PORT_RCV, CTRL_PORT_RCV_DEFAULT);
    std::string droneAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_rcvSocket.open(droneAddr.c_str(), m_droneSendPort, "", myRcvPort) == -1) {
		logE << "UDP open error" << std::endl;
        return -1;
	}

	return 1;
}

void PCReceiver::innerRun(NetFrame& netFrame) {

}