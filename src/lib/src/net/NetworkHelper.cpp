#include "net/NetworkHelper.h"

#include <string.h>
#include <stdarg.h>
#include <iostream>

namespace net {

std::string NetworkHelper::getIpv4Addr(struct sockaddr_in &sa) {
	char addr[16];
	inet_ntop(AF_INET, &sa.sin_addr, addr, 16);
	return std::string(addr);
}

void NetworkHelper::writeUInt32(UINT32 value, UINT8* buffer, int index) {
	buffer[index++] = (UINT8)(value & 0xff);
	buffer[index++] = (UINT8)((value & 0xff00) >> 8);
	buffer[index++] = (UINT8)((value & 0xff0000) >> 16);
	buffer[index] = (UINT8)((value & 0xff000000) >> 24);
}

void NetworkHelper::writeUInt16(UINT16 value, UINT8* buffer, int index) {
	buffer[index++] = (UINT8)(value & 0xff);
	buffer[index] = (UINT8)((value & 0xff00) >> 8);
}

void NetworkHelper::writeFrame(const NetworkFrame& frame, UINT8* buffer) {
	writeFrameHeader(frame, buffer);
	memcpy(&buffer[FRAME_HEADER], frame.data, frame.dataSize);
}

void NetworkHelper::writeFrame(const NetworkFrame& frame, UINT8* buffer, const char* format, ...) {
	writeFrameHeader(frame, buffer);
	
	va_list args;
	UINT8 count = FRAME_HEADER;
	UINT8 v1; UINT16 v2; UINT32 v3; UINT8 b;

	va_start(args, format); {
		while (*format != '\0' && count < frame.totalSize) {
			switch (*format++) {
			case 'b':
				b = (UINT8)va_arg(args, int);
				buffer[count++] = b;
				break;
			case '1':
				v1 = (UINT8)va_arg(args, int);
				buffer[count++] = v1;
				break;
			case '2':
				v2 = (UINT16)va_arg(args, int);
				writeUInt16(v2, buffer, count);
				count += 2;
				break;
			case '4':
				v3 = va_arg(args, int);
				writeUInt32(v3, buffer, count);
				count += 4;
				break;
			}
		}
	} va_end(args);
}

void NetworkHelper::writeFrameHeader(const NetworkFrame& frame, UINT8* buffer) {
	buffer[0] = frame.type;
	buffer[1] = frame.id;
	buffer[2] = frame.seq;
	writeUInt32(frame.totalSize, buffer, 3);
}

UINT32 NetworkHelper::readUInt32(const char *buffer, int index) {
	return (UINT32)((UINT8)(buffer[index] << 0) | ((UINT8)buffer[index + 1] << 8) |
		((UINT8)buffer[index + 2] << 16) | ((UINT8)buffer[index + 3] << 24));
}

UINT32 NetworkHelper::readUInt32(const UINT8 *buffer, int index) {
	return (UINT32)((buffer[index] << 0) | (buffer[index + 1] << 8) |
		(buffer[index + 2] << 16) | (buffer[index + 3] << 24));
}

UINT16 NetworkHelper::readUInt16(const char *buffer, int index) {
	return (UINT16)((UINT8)(buffer[index] << 0) | (UINT8)(buffer[index + 1] << 8));
}

UINT16 NetworkHelper::readUInt16(const UINT8 *buffer, int index) {
	return (UINT16)((buffer[index] << 0) | (buffer[index + 1] << 8));
}

std::string NetworkHelper::readString(const char *buffer, int index) {
	std::string str;
	while (*buffer != '\0') {
		str += *buffer++;
	}
	return str;
}

void NetworkHelper::readFrame(const char* buffer, NetworkFrame& frame) {
	frame.type = (UINT8)buffer[0];
	frame.id = (UINT8)buffer[1];
	frame.seq = (UINT8)buffer[2];
	frame.totalSize = readUInt32(buffer, 3);
	frame.dataSize = frame.totalSize - FRAME_HEADER;

	if (frame.totalSize > FRAME_HEADER) {
		frame.data = new UINT8[frame.dataSize];
		memcpy(frame.data, &buffer[FRAME_HEADER], frame.dataSize);
	}
}

void NetworkHelper::readArgs(const NetworkFrame& frame, const char* format, ...) {
	va_list args;
	UINT8 count = 0;
	UINT8* v1; UINT16* v2; UINT32* v3; UINT8* b;

	va_start(args, format); {
		while (*format != '\0' && count < frame.dataSize) {
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
				v2 = (UINT16*)va_arg(args, int*);
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
