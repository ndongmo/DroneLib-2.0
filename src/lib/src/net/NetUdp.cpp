#include "net/NetUdp.h"
#include "utils/Logger.h"

#ifdef _WIN32 
#pragma warning(disable: 4996)
#endif

namespace net {

NetUdp::NetUdp(void) {
#ifdef _WIN32 
	WSADATA wsaData;
	// initiates use of the Winsock DLL by our process.
	int err = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (err != 0) {
		logE << "udp client" << std::endl;
		switch (err) {
		case WSASYSNOTREADY:
			logE << "WSASYSNOTREADY" << std::endl;
			break;
		case WSAVERNOTSUPPORTED:
			logE << "WSAVERNOTSUPPORTED" << std::endl;
			break;
		case WSAEINPROGRESS:
			logE << "WSAEINPROGRESS" << std::endl;
			break;
		case WSAEPROCLIM:
			logE << "WSAEPROCLIM" << std::endl;
			break;
		case WSAEFAULT:
			logE << "WSAEFAULT" << std::endl;
			break;
		}
	}
#endif
	m_sock = INVALID_SOCKET;
}

NetUdp::~NetUdp(void) {
	close();
}

int NetUdp::open(const char *clientAddr, int clientPort, const char *serverAddr, int serverPort) {
	if ((m_sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		logE << "socket creation failed:" << strerror(errno) << std::endl;
		return -1;
	}

	//setting client port
	m_client_addr.sin_family = AF_INET;
	m_client_addr.sin_port = htons(clientPort);

	if(inet_pton(AF_INET, clientAddr, &m_client_addr.sin_addr.s_addr) < 1) {
		logE << "wrong client address given: " << clientAddr << std::endl;
		return -1;
	}

	//setting server port
	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(serverPort);

	if(strlen(serverAddr) == 0) {
		m_server_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else if(inet_pton(AF_INET, serverAddr, &m_server_addr.sin_addr.s_addr) < 1) {
		logE << "wrong server address given: " << serverAddr << std::endl;
		return -1;
	}

	// The bind function is required on an unconnected socket
	if (bind(m_sock, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) == SOCKET_ERROR) {
		logE << "binding failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return 1;
}

int NetUdp::send(const char *buf, int size) {
	if (m_sock == INVALID_SOCKET) {
		logE << "socket unavailable" << std::endl;
		return -1;
	}

	// Sends a datagram to the client address
	int len = sendto(m_sock, buf, size, 0, (struct sockaddr *)&m_client_addr, sizeof(m_client_addr));

	if (len == SOCKET_ERROR) {
		logE << "udp sending failed: " << strerror(errno) << std::endl;
		return -1;
	}
	
	return len;
}

int NetUdp::receive(char *buf, int size) {
	if (m_sock == INVALID_SOCKET) {
		logE << "socket unavailable" << std::endl;
		return -1;
	}

	socklen_t addrLen = sizeof(m_server_addr);

	// receive datagrams on the bound socket
	int len = recvfrom(m_sock, buf, size, 0, (struct sockaddr *)&m_server_addr, &addrLen);

	if (len == SOCKET_ERROR) {
		logE << "receive failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return len;
}

void NetUdp::close() {
#ifdef _WIN32 
	if (m_sock != INVALID_SOCKET) {
		shutdown(m_sock, SD_BOTH);
		closesocket(m_sock);
	}
	WSACleanup(); // terminates use of the Winsock 2 DLL
#else
	if (m_sock != INVALID_SOCKET) {
		::shutdown(m_sock, SHUT_RDWR);
		::close(m_sock);
	}
#endif
	m_sock = INVALID_SOCKET;
}

bool NetUdp::isOpen() {
	return m_sock != INVALID_SOCKET;
}
}