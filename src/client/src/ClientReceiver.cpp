#include "ClientReceiver.h"
#include "ClientSender.h"
#include "BatteryReceiver.h"

#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>
#include <stream/StreamReceiver.h>

using namespace utils; 

ClientReceiver::ClientReceiver(BatteryReceiver &batReceiver, ClientSender &sender, 
        StreamReceiver &videoReceiver, StreamReceiver &audioReceiver) : 
	NetReceiver(sender), m_batReceiver(batReceiver), m_clientSender(sender), 
	m_videoReceiver(videoReceiver), m_audioReceiver(audioReceiver) {
	m_name = "ClientReceiver";
}

int ClientReceiver::begin() {
    int rcvPort = Config::getInt(CLIENT_PORT_RCV);
	int droneSendPort = Config::getInt(DRONE_PORT_SEND);
    std::string droneAddr = Config::getString(DRONE_ADDRESS);

	if (m_rcvSocket.open(droneAddr.c_str(), droneSendPort, "", rcvPort) == -1) {
		logE << "UDP open error" << std::endl;
        return -1;
	}

	return 1;
}

void ClientReceiver::innerRun(NetFrame &netFrame) {
	if(netFrame.type == NS_FRAME_TYPE_DATA) {
		if (netFrame.id == NS_ID_STREAM_VIDEO) {
			StreamFragment streamFragment;
			net::NetHelper::readFrame(netFrame, streamFragment);
			m_videoReceiver.newFragment(streamFragment);
		} 
		else if (netFrame.id == NS_ID_STREAM_AUDIO) {
			StreamFragment streamFragment;
			net::NetHelper::readFrame(netFrame, streamFragment);
			m_audioReceiver.newFragment(streamFragment);
		}
		else if (netFrame.id == NS_ID_BATTERY_LIFE) {
			m_batReceiver.newBatteryLife((int)netFrame.data[0]);
		}
	}
}