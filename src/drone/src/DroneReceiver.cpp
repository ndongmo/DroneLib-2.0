#include "DroneReceiver.h"
#include "Constants.h"

#include <IController.h>
#include <net/NetTcp.h>
#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Structs.h>
#include <utils/Constants.h>

using namespace utils; 

DroneReceiver::DroneReceiver(DroneSender& sender, MotorController& motorCtrl) : 
	NetReceiver(sender), m_droneSender(sender), m_motorCtrl(motorCtrl) {

}

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
	if(netFrame.type == NS_FRAME_TYPE_DATA) {
		if (netFrame.id == NS_ID_NAV) {
			int dir = (int)netFrame.data[0];
			int speed = NetHelper::readUInt16(netFrame.data, 1);
			m_motorCtrl.move((DroneDir)dir, (DroneSpeed)speed);
		}
	}
}