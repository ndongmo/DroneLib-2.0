/*!
* \file NetTcp.h
* \brief TCP connection implementation.
* \author Ndongmo Silatsa
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
 * Keep a socket and implement TCP open, send and receive methods.
 * It implements the TCP server which listen to only one client and 
 * close the listening socket once this client is connected.
 */
class NetTcp
{
public:
	NetTcp(void);
	~NetTcp(void);

	/**
	 * Initialize a client TCP connection using the given sever parameters.
	 * \param addr : server address
	 * \param port : server port number
	 * \return -1 when the connection failed, otherwise 1.
	 */
	int openClient(const char *addr, int port);

	/**
	 * Initialize a sever TCP connection using the given parameters.
	 * If the given listened address is empty, the socket will listen to INADDR_ANY.
	 * \param addr : listened address, if empty the socket will listen to INADDR_ANY
	 * \param port : listened port number
	 * \return -1 when the connection failed, otherwise 1.
	 */
	int openServer(const char *addr, int port);

	/**
	 * Listen the opened socket and accept the first incoming client connection.
	 * \param client : client address struct
	 * \return -1 when the connection failed, otherwise 1.
	 */
	int listen(struct sockaddr_in &client);

	/**
	* Send data to the server.
	* \param buf : sending buffer
	* \param size : buffer size
	* \return -1 when an error occured, otherwise the lenght of sent data
	*/
	int send(const char *buf, int size);

	/**
	* Receive data from the server.
	* \param buf : destination buffer
	* \param size : destination buffer size
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
     * server address.
     */
	struct sockaddr_in m_server_addr;
};
}
