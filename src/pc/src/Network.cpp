#include "Network.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils; 

int Network::begin() {
	if (discovery() == -1) {
		logE << "Discovery error" << std::endl;
		return -1;
	}

    int myPort = Config::getInt(CTRL_PORT_RCV, CTRL_PORT_RCV_DEFAULT);
    std::string droneAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_udp.open(droneAddr.c_str(), m_droneRcvPort, "", myPort) == -1) {
		logE << "UDP open error" << std::endl;
        return -1;
	}

	return 1;
}

int Network::discovery() {
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_tcp.openClient(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open client error" << std::endl;
        return -1;
	}
	
    nlohmann::json json = {
        {CTRL_PORT_RCV, Config::getInt(CTRL_PORT_RCV, CTRL_PORT_RCV_DEFAULT)}
    };
    std::string msg = json.dump();

	if (m_tcp.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send PC config failed" << std::endl;
		return -1;
	}

	char buf[1024];

	if (m_tcp.receive(buf, 1024) == -1) {
		logE << "Discovery: TCP receive drone config failed" << std::endl;
		return -1;
	}

	try {
        json = nlohmann::json::parse(buf);
		json[DRONE_PORT_RCV].get_to(m_droneRcvPort);
		json[NET_FRAGMENT_SIZE].get_to(m_maxFragmentSize);
        json[NET_FRAGMENT_NUMBER].get_to(m_maxFragmentNumber);
	}
	catch (...) {
		logE << "Json parser error" << std::endl;
		return -1;
	}

	return 1;
}

void Network::innerRun(NetworkFrame& NetworkServiceFrame) {

}