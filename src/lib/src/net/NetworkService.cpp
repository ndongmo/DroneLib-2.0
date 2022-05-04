#include "net/NetworkService.h"
#include "utils/Logger.h"
#include "utils/Config.h"

using namespace utils;

namespace net {

int NetworkService::end() {
	m_process.join();

	return 1;
}

void NetworkService::start() {
	m_process = std::thread([this]{run(); });
}

void NetworkService::run() {
	int len;
	char *buf = new char[m_maxFragmentSize];
	NetworkFrame networkFrame;

	while (true) {
		memset(buf, 0, m_maxFragmentSize);			 // buffer initialization
		len = m_udp.receive(buf, m_maxFragmentSize); //receive data
		if (len == -1) {
			logE << "NetworkService receive socket error" << std::endl;
			return;
		}

		NetworkHelper::readFrame(buf, networkFrame);
		innerRun(networkFrame);

		delete[] networkFrame.data;
	}
	delete[] buf;
}
} // namespace net