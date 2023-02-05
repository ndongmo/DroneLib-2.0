#include "PCReceiver.h"
#include "Constants.h"

#include <net/NetHelper.h>
#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils; 

PCReceiver::PCReceiver(PCSender &sender, StreamReceiver &videoReceiver, 
	StreamReceiver &audioReceiver) : 
	NetReceiver(sender), m_pcSender(sender), m_videoReceiver(videoReceiver), 
	m_audioReceiver(audioReceiver) {

}

int PCReceiver::begin() {
    int rcvPort = Config::getInt(CTRL_PORT_RCV);
	int droneSendPort = Config::getInt(DRONE_PORT_SEND);
    std::string droneAddr = Config::getString(DRONE_ADDRESS);

	if (m_rcvSocket.open(droneAddr.c_str(), droneSendPort, "", rcvPort) == -1) {
		logE << "UDP open error" << std::endl;
        return -1;
	}

	return 1;
}

void PCReceiver::innerRun(NetFrame &netFrame) {
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
	}
}