/*!
* \file NetUdp.h
* \brief UDP connection implementation.
* \author Ndongmo Silatsa Fabrice
* \date 26-02-2019
* \version 1.0
*/

#pragma once

#include <iostream>

#ifdef _WIN32 
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

typedef int SOCKET;
#endif

namespace net
{
/**
 * Keep a socket and implement UDP open, send and receive methods.
 */
class NetUdp
{
public:
	NetUdp(void);
	~NetUdp(void);
	
	/**
	 * Initialize an UDP connection with the given parameters.
	 * Make sure the server address exists in the local network.
	 * If the given server address is empty, the server will listen to INADDR_ANY.
	 * \param clientAddr : client address
	 * \param clientPort : client port number
	 * \param serverAddr : server address, if empty the server will listen to INADDR_ANY
	 * \param serverPort : server port number
	 * \return -1 when an error occured, otherwise 1.
	 */
	int open(const char *clientAddr, int clientPort, const char *serverAddr, int serverPort);

	/**
	 * Sends data to the client address.
	 * \param buf : data buffer
	 * \param size : buffer size
	 * \return -1 when an error occured, otherwise the lenght of sent data
	 */
	int send(const char *buf, int size);

	/**
	 * Receives data from the client address.
	 * \param buf : data buffer
	 * \param size : buffer size
	 * \return -1 when an error occured, otherwise the lenght of received data
	 */
	int receive(char *buf, int size);

	/**
	 * Close the current socket. If the socket was blocked in recvfrom function, 
	 * the function will throw an error.
	 */
	void close();

	/**
	 *  Check if the current socket is opened.
	 * \return true if the socket is opened, false otherwise. 
	 */
	bool isOpen();

private:
	/**
     * Current socket.
     */
	SOCKET m_sock;
	/**
     * Server socket address.
     */
	struct sockaddr_in m_server_addr;
	/**
     * Client socket address.
     */
	struct sockaddr_in m_client_addr;
};
}
