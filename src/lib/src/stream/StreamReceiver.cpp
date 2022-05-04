#include "stream/StreamReceiver.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

using namespace utils;

namespace stream {

int StreamReceiver::end() {
	delete[] m_avFrame.frame;
	delete[] m_avFrame.rcvWindow;

	return 1;
}

void StreamReceiver::start() {
	m_ackStreamFragment = Config::getInt(STREAM_FRAGMENT_ACKED, NS_STREAM_ACK);
}

void StreamReceiver::run() {
	
}

UINT8* StreamReceiver::newFrame(const NetFrame& netFrame) {
	StreamFragment streamFragment;

	net::NetHelper::readFrame(netFrame, streamFragment);

	// received new frame
	if (streamFragment.frameNumber != m_avFrame.frameNumber) {
		// push the frame if the last frame has at least one fragment and
		// the frameFlags is 1 (override) or the incoming frame is new
		if (m_avFrame.fragmentNumber > 0 && 
			(m_avFrame.frameFlags == 1 || streamFragment.frameNumber > m_avFrame.frameNumber)) {
			m_rcvMtx.lock();
			m_pool.add(m_avFrame.frame, m_avFrame.frameSize);
			m_rcvMtx.unlock();
		}

		int oldFrameSize =  m_avFrame.frameSize;
		
		m_avFrame.fragmentNumber = 0;
		m_avFrame.frameNumber = streamFragment.frameNumber;
		m_avFrame.frameSize = streamFragment.fragmentPerFrame * streamFragment.fragmentSize;
		m_avFrame.frameFlags = streamFragment.frameFlags;
		m_avFrame.fragmentPerFrame = streamFragment.fragmentPerFrame;

		if(oldFrameSize != m_avFrame.frameSize) {
			delete[] m_avFrame.frame;
			m_avFrame.frame = new UINT8[m_avFrame.frameSize];
		}
		memset(m_avFrame.frame, 0, m_avFrame.frameSize);
		memset(m_avFrame.rcvWindow, 0, STREAM_FRAME_ACK);
	}

	memcpy(&m_avFrame.frame[streamFragment.fragmentNumber], streamFragment.fragmentData, streamFragment.fragmentSize);
	if(streamFragment.fragmentNumber > m_avFrame.fragmentNumber) {
		m_avFrame.fragmentNumber = streamFragment.fragmentNumber;
	}
	
	if(m_ackStreamFragment) {
		// fill the reception window and send it to ack the received fragment
		int bufferPosition = (int)floor(streamFragment.fragmentNumber / 8 | 0);
		m_avFrame.rcvWindow[2 + bufferPosition] |= (UINT8)(1 << streamFragment.fragmentNumber % 8);
		net::NetHelper::writeUInt16(streamFragment.frameNumber, m_avFrame.rcvWindow, 0);
		return m_avFrame.rcvWindow;
	}
	return nullptr;
}
} // namespace net