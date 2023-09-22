#include "DroneReceiver.h"
#include "DroneSender.h"
#include "Constants.h"
#include "controller/MotorController.h"
#include "controller/ServoController.h"
#include "controller/BuzzerController.h"

#include <IController.h>
#include <net/NetTcp.h>
#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Structs.h>
#include <utils/Constants.h>

using namespace utils; 

DroneReceiver::DroneReceiver(DroneSender& sender, MotorController& motorCtrl, 
	ServoController& servoCtrl, BuzzerController& buzzerCtrl) : 
	NetReceiver(sender), m_droneSender(sender), m_motorCtrl(motorCtrl), 
	m_servoCtrl(servoCtrl), m_buzzerCtrl(buzzerCtrl) {
	m_name = "DroneReceiverService";
}

int DroneReceiver::begin() {
	int dronePort = Config::getInt(DRONE_PORT_RCV);
	int clienRcvPort = Config::getInt(CLIENT_PORT_RCV);
    std::string droneAddr = Config::getString(DRONE_ADDRESS);
	std::string clientAddr = Config::getString(CLIENT_ADDRESS);

	if (m_rcvSocket.open(clientAddr.c_str(), clienRcvPort, droneAddr.c_str(), dronePort) == -1) {
		logE << "DroneReceiver: UDP open error" << std::endl;
        return -1;
	}

	return 1;
}

void DroneReceiver::innerRun(NetFrame& netFrame) {
	if(netFrame.type == NS_FRAME_TYPE_ACK) {
		if (netFrame.id == NS_ID_STREAM_VIDEO) {
			
		}
	}
	else if(netFrame.type == NS_FRAME_TYPE_DATA) {
		if (netFrame.id == NS_ID_NAV) {
			int dir = (int)netFrame.data[0];
			int speed = NetHelper::readUInt16(netFrame.data, 1);
			m_motorCtrl.move((DroneDir)dir, (DroneSpeed)speed);
		}
		else if (netFrame.id == NS_ID_CAMERA) {
			int axe = (int)netFrame.data[0];
			int angle = (int)netFrame.data[1];
			int sign = (int)netFrame.data[2];
			if(sign == 0) angle = -angle;
			m_servoCtrl.rotate((DroneCamera)axe, angle);
		}
		else if (netFrame.id == NS_ID_BUZZER) { 
			m_buzzerCtrl.turn(true);
		}
	}
}