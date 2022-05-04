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

    int myPort = Config::getInt(DRONE_PORT_RCV, DRONE_PORT_RCV_DEFAULT);
    std::string myAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	if (m_udp.open(m_clientAddr.c_str(), m_clientRcvPort, myAddr.c_str(), myPort) == -1) {
		logE << "UDP open error" << std::endl;
        return -1;
	}

	return 1;
}

int Network::discovery() {
    int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
    std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);
	struct sockaddr_in client;

	if (m_tcp.openServer(serverAddr.c_str(), serverPort) == -1) {
		logE << "Discovery: TCP open server error" << std::endl;
        return -1;
	}

	if (m_tcp.listen(client) == -1) {
		logE << "Discovery: TCP listen client error" << std::endl;
        return -1;
	}

	m_clientAddr = NetworkHelper::getIpv4Addr(client);

	char buf[1024];
	nlohmann::json json;

	if (m_tcp.receive(buf, 1024) == -1) {
		logE << "Discovery: TCP receive client config failed" << std::endl;
		return -1;
	}

	try {
        json = nlohmann::json::parse(buf);
		json[CTRL_PORT_RCV].get_to(m_clientRcvPort);
	}
	catch (...) {
		logE << "Discovery: Json parser error" << std::endl;
		return -1;
	}

	m_maxFragmentSize = Config::getInt(NET_FRAGMENT_SIZE, NET_FRAGMENT_SIZE_DEFAULT);
	m_maxFragmentNumber = Config::getInt(NET_FRAGMENT_NUMBER, NET_FRAGMENT_NUMBER_DEFAULT);
	
    json = {
        {DRONE_PORT_RCV, Config::getInt(DRONE_PORT_RCV, DRONE_PORT_RCV_DEFAULT)},
		{NET_FRAGMENT_SIZE, m_maxFragmentSize},
		{NET_FRAGMENT_NUMBER, m_maxFragmentNumber}
    };
    std::string msg = json.dump();

	if (m_tcp.send(msg.c_str(), msg.length()) == -1) {
		logE << "Discovery: TCP send config failed" << std::endl;
		return -1;
	}

	return 1;
}

void Network::innerRun(NetworkFrame& NetworkServiceFrame) {

}