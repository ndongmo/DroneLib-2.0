#include "net/NetReceiver.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"
#include "IController.h"

using namespace utils;

namespace net {

NetReceiver::NetReceiver(NetSender& sender) : m_sender(sender) {
	m_name = "NetReciver";
}

int NetReceiver::end() {
	m_running = false;
	is_closing = true;
	
	if(m_rcvSocket.isOpen()) {
		m_rcvSocket.close();
	}
	if(m_process.joinable()) {
		m_process.join();
	}

	return 1;
}

void NetReceiver::start() {
	m_running = true;
	m_process = std::thread([this]{run(); });
}

void NetReceiver::run() {
	int len = 0;
	int maxFragmentSize = Config::getInt(NET_FRAGMENT_SIZE);
	char* buf = new char[maxFragmentSize];

	while (m_running) {
		memset(buf, 0, maxFragmentSize);

		len = m_rcvSocket.receive(buf, maxFragmentSize);

		if (len < 1) {
			if(len == 0) {
				logI << "NetReceiver: the other side closes the connection" << std::endl;
			} else {
				logE << "NetReceiver receives socket error" << std::endl;
			}
			m_running = false;
			continue;
		}

		NetFrame netFrame;
		NetHelper::readFrame(buf, netFrame);

		if (netFrame.type == NS_FRAME_TYPE_QUIT) {
			if(m_controller != nullptr) {
				m_controller->updateState(APP_CLOSING);
			}
			continue;
		}

		if (netFrame.type == NS_FRAME_TYPE_DATA_WITH_ACK) {
			m_sender.sendAck(netFrame.seq, netFrame.id);
		}
		if (netFrame.id == NS_ID_PING) {
			m_sender.sendPong(netFrame.seq);
		} else {
			innerRun(netFrame);
		}
	}
	if(len < 1 && !is_closing) {
		sendError(ERROR_NET_RECEIVE);
	}

	delete[] buf;
}

bool NetReceiver::isConnected() {
	return m_rcvSocket.isOpen();
}
} // namespace net