#include "net/NetHelper.h"

#include <string.h>
#include <stdarg.h>

namespace net {

std::string NetHelper::getIpv4Addr(struct sockaddr_in &sa) {
	char addr[16];
	inet_ntop(AF_INET, &sa.sin_addr, addr, 16);
	return std::string(addr);
}

void NetHelper::writeUInt32(UINT32 value, UINT8* buffer, int index) {
	buffer[index++] = (UINT8)(value & 0xff);
	buffer[index++] = (UINT8)((value & 0xff00) >> 8);
	buffer[index++] = (UINT8)((value & 0xff0000) >> 16);
	buffer[index] = (UINT8)((value & 0xff000000) >> 24);
}

void NetHelper::writeUInt16(UINT16 value, UINT8* buffer, int index) {
	buffer[index++] = (UINT8)(value & 0xff);
	buffer[index] = (UINT8)((value & 0xff00) >> 8);
}

UINT32 NetHelper::readUInt32(const char *buffer, int index) {
	return (UINT32)((UINT8)(buffer[index] << 0) | ((UINT8)buffer[index + 1] << 8) |
		((UINT8)buffer[index + 2] << 16) | ((UINT8)buffer[index + 3] << 24));
}

UINT32 NetHelper::readUInt32(const UINT8 *buffer, int index) {
	return (UINT32)((buffer[index] << 0) | (buffer[index + 1] << 8) |
		(buffer[index + 2] << 16) | (buffer[index + 3] << 24));
}

UINT16 NetHelper::readUInt16(const char *buffer, int index) {
	return (UINT16)((UINT8)(buffer[index] << 0) | (UINT8)(buffer[index + 1] << 8));
}

UINT16 NetHelper::readUInt16(const UINT8 *buffer, int index) {
	return (UINT16)((buffer[index] << 0) | (buffer[index + 1] << 8));
}

std::string NetHelper::readString(const char *buffer, int index) {
	std::string str;
	while (*buffer != '\0') {
		str += *buffer++;
	}
	return str;
}

void NetHelper::readFrame(const char* buffer, NetFrame& frame) {
	frame.type = (UINT8)buffer[0];
	frame.id = (UINT8)buffer[1];
	frame.seq = (UINT8)buffer[2];
	frame.size = readUInt32(buffer, 3);
	frame.data = (UINT8*) (buffer + NET_FRAME_HEADER);
}

void NetHelper::readFrame(const NetFrame& frame, StreamFragment& fragment) {
	fragment.frameFlags = frame.data[0];
	fragment.frameNumber = readUInt16(frame.data, 1);
	fragment.fragmentNumber = readUInt16(frame.data, 3);
	fragment.frameSize = readUInt32(frame.data, 5);
	fragment.fragmentSize = frame.size - NET_FRAME_HEADER - STREAM_FRAME_HEADER;
	fragment.fragmentData = frame.data + STREAM_FRAME_HEADER;
}

void NetHelper::readArgs(const NetFrame& frame, const char* format, ...) {
	va_list args;
	UINT8 count = 0;
	UINT8* v1; int* v2; UINT32* v3; UINT8* b;

	va_start(args, format); {
		while (*format != '\0' && count < frame.size - NET_FRAME_HEADER) {
			switch (*format++) {
			case 'b':
				b = (UINT8*)va_arg(args, int*);
				*b = frame.data[count++];
				break;
			case '1':
				v1 = (UINT8*)va_arg(args, int*);
				*v1 = frame.data[count++];
				break;
			case '2':
				v2 = va_arg(args, int*);
				*v2 = readUInt16(frame.data, count);
				count += 2;
				break;
			case '4':
				v3 = (UINT32*)va_arg(args, int*);
				*v3 = readUInt32(frame.data, count);
				count += 4;
				break;
			}
		}
	} va_end(args);
}
} // namespace name
