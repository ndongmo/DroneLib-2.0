#include "net/NetTcp.h"
#include "utils/Logger.h"

#ifdef _WIN32 
#pragma warning(disable: 4996)
#endif

namespace net {

NetTcp::NetTcp(void) {
#ifdef _WIN32 
	WSADATA wsaData;
	// initiates use of the Winsock DLL by our process.
	int err = WSAStartup(MAKEWORD(2, 0), &wsaData); 
	if (err != 0) {
		logE << "tcp client";
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

NetTcp::~NetTcp(void) {
	close();
}

int NetTcp::openClient(const char *addr, int port) {
	if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		logE << "socket creation failed: " << strerror(errno) << std::endl;
		return -1;
	}

	int opt = 1;
	// Forcefully attaching socket to the port 8080
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        logE << "socket set option failed: " << strerror(errno) << std::endl;
		return -1;
    }

	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, addr, &m_server_addr.sin_addr.s_addr) < 1) {
		logE << "wrong server address given: " << strerror(errno) << std::endl;
		return -1;
	}

	if (connect(m_sock, (const sockaddr *)&m_server_addr, sizeof(m_server_addr)) == SOCKET_ERROR) {
		logE << "socket connection failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return 1;
}

int NetTcp::openServer(const char *addr, int port) {
	if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		logE << "socket creation failed: " << strerror(errno) << std::endl;
		return -1;
	}

	int opt = 1;
	// Forcefully attaching socket to the port 8080
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        logE << "socket set option failed: " << strerror(errno) << std::endl;
		return -1;
    }

	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(port);

	if(strlen(addr) == 0) {
		m_server_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else if(inet_pton(AF_INET, addr, &m_server_addr.sin_addr.s_addr) < 1) {
		logE << "wrong server address given: " << strerror(errno) << std::endl;
		return -1;
	}

	if (bind(m_sock, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) == SOCKET_ERROR) {
		logE << "socket binding failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return 1;
}

int NetTcp::listen(struct sockaddr_in &client) {
	int addrlen = sizeof(client);
	int new_sock;

	if (::listen(m_sock, 1) < 0) {
        logE << "Tcp listen failed: " << strerror(errno) << std::endl;
		return -1;
    }
    if ((new_sock = accept(m_sock, (struct sockaddr*)&client, (socklen_t*)&addrlen)) < 0) {
        logE << "Tcp accept failed: " << strerror(errno) << std::endl;
		return -1;
    }
	// Our TCP server only handle one client, so we close the listening socket and 
	// start working with the new opened client socket!!!
#ifdef _WIN32 
	closesocket(m_sock);
#else
	::close(m_sock);
#endif
	m_sock = new_sock;

	return 1;
}

int NetTcp::receive(char *buf, int size) {
	if (m_sock == INVALID_SOCKET) {
		logE << "socket unavailable" << std::endl;
		return -1;
	}

	int len = recv(m_sock, buf, size, 0);

	if (len == SOCKET_ERROR) {
		logE << "tcp reception failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return len;
}

int NetTcp::send(const char *buf, int size) {
	if (m_sock == INVALID_SOCKET) {
		logE << "socket unavailable" << std::endl;
		return -1;
	}

	int len = ::send(m_sock, buf, size, 0);
	
	if (len == SOCKET_ERROR) {
		logE << "tcp sending failed: " << strerror(errno) << std::endl;
		return -1;
	}

	return len;
}

void NetTcp::close() {
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

bool NetTcp::isOpen() {
	return m_sock != INVALID_SOCKET;
}
}