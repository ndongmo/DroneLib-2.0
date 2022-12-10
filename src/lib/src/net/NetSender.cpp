#include "net/NetSender.h"
#include "utils/Constants.h"
#include "utils/Config.h"
#include "utils/Logger.h"

#include <string.h>
#include <stdarg.h>

using namespace utils;

namespace net {

NetSender::NetSender() {
	addCommand(NS_ID_PING, NS_FREQ_PING);
}

int NetSender::end() {
	if(m_seqBuf != nullptr) {
		delete[] m_seqBuf;
		m_seqBuf = nullptr;
	}
	if(m_buffer != nullptr) {
		delete[] m_buffer;
		m_buffer = nullptr;
	}
	if(m_sendSocket.isOpen()) {
		m_sendSocket.close();
	}

	return 1;
}

void NetSender::sendFrame(int id, int type, int length, UINT8* data) {
	int totalSize = length + NET_FRAME_HEADER;

	m_sendMtx.lock();

	writeFrameHeader(id, type, totalSize);
	memcpy(&m_buffer[NET_FRAME_HEADER], data, length);

	m_sendSocket.send((char*)m_buffer, totalSize);
	m_sendMtx.unlock();
}

void NetSender::sendFrame(int id, int type, const char* format, ...) {
	int totalSize = getLength(format) + NET_FRAME_HEADER;
	
	m_sendMtx.lock();
	writeFrameHeader(id, type, totalSize);
	
	va_list args;
	UINT8 count = NET_FRAME_HEADER;
	UINT8 v1; UINT16 v2; UINT32 v3; UINT8 b;

	va_start(args, format); {
		while (*format != '\0' && count < totalSize) {
			switch (*format++) {
			case 'b':
				b = (UINT8)va_arg(args, int);
				m_buffer[count++] = b;
				break;
			case '1':
				v1 = (UINT8)va_arg(args, int);
				m_buffer[count++] = v1;
				break;
			case '2':
				v2 = (UINT16)va_arg(args, int);
				NetHelper::writeUInt16(v2, m_buffer, count);
				count += 2;
				break;
			case '4':
				v3 = va_arg(args, int);
				NetHelper::writeUInt32(v3, m_buffer, count);
				count += 4;
				break;
			}
		}
	} va_end(args);
	
	logI << "NetSender::Netframe type["<< (int)m_buffer[0] << "] id[" << (int)m_buffer[1] << "] seq[" << 
		(int)m_buffer[2] << "] size[" << totalSize << "]" << std::endl;

	totalSize = m_sendSocket.send((char*)m_buffer, totalSize);
	m_sendMtx.unlock();

	if(totalSize == -1) {
		sendError(ERROR_NET_SEND);
	}
}

void NetSender::writeFrameHeader(int id, int type, int totalSize) {
	m_buffer[0] = type;
	m_buffer[1] = id;
	m_buffer[2] = getNextSeqID(id);
	NetHelper::writeUInt32(totalSize, m_buffer, 3);
}

int NetSender::getLength(const char* format) {
	int length = 0;
	while (*format != '\0') {
		if (*format == 'b') length += 1;
		else length += *format - '0';

		format++;
	}
	return length;
}

UINT8 NetSender::getNextSeqID(int id) {
	m_seqBuf[id]++;
	if (m_seqBuf[id] >= m_maxFragmentNumber) {
		 m_seqBuf[id] = 0;
	}
	return m_seqBuf[id];
}

void NetSender::addCommand(int id, unsigned int frequency) {
	m_freqs[id] = frequency;
	m_currentFreqs[id] = 0;
}

bool NetSender::canSend(int id, int deltatime) {
	if(m_freqs.find(id) != m_freqs.end()) {
		return m_freqs[id] <= m_currentFreqs[id] + deltatime;
	}
	return true;
}

void NetSender::sendAck(UINT8 id, UINT8 seq) {
    sendFrame(id, NS_FRAME_TYPE_ACK, "1", seq);
}

void NetSender::sendPing(int deltatime, UINT8 seq) {
	if(canSend(NS_ID_PING, deltatime)) {
		sendFrame(NS_ID_PING, NS_FRAME_TYPE_DATA, "1", seq);
		m_currentFreqs[NS_ID_PING] = 0;
	} else {
		m_currentFreqs[NS_ID_PING] += deltatime;
	}
}

void NetSender::sendPong(UINT8 seq) {
    sendFrame(NS_ID_PONG, NS_FRAME_TYPE_DATA, "1", seq);
}

}