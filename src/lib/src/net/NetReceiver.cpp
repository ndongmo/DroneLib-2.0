#include "net/NetReceiver.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

using namespace utils;

namespace net {

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
	char buf[m_maxFragmentSize];

	while (m_running) {
		memset(buf, 0, m_maxFragmentSize);

		len = m_rcvSocket.receive(buf, m_maxFragmentSize);
		if (len == -1) {
			logE << "NetReceiver receive socket error" << std::endl;
			m_running = false;
			continue;
		}

		NetFrame netFrame;
		NetHelper::readFrame(buf, netFrame);
		innerRun(netFrame);
	}
	if(len == -1 and !is_closing) {
		sendError(ERROR_NET_RECEIVE);
	}
}
} // namespace net