#include "stream/StreamReceiver.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Constants.h"

using namespace utils;

namespace stream {

int StreamReceiver::begin() {
	m_frame.frameNumber = -1;
	return 1;
}

void StreamReceiver::start() {
	m_running = true;
}

void StreamReceiver::run() {
	
}

int StreamReceiver::end() {
	m_running = false;
	return 1;
}

void StreamReceiver::updateFrame() {
	std::lock_guard<std::mutex> guard(m_rcvMtx);
	if(m_currentFrame != m_oldFrame) {
		m_newFrame = true;
		m_oldFrame = m_currentFrame;
		m_size = m_pool.getSize(m_oldFrame);
		m_data = m_pool.getData(m_oldFrame);
	}
}

UINT8* StreamReceiver::newFragment(const StreamFragment &frag) {
	// received new frame
	if (frag.frameNumber != m_frame.frameNumber) {
		// push the frame if the last frame has at least one fragment and
		// the frameFlags is 1 (override) or the incoming frame is new
		if (m_frame.fragmentNumber > 0 &&
			(m_frame.frameFlags == 1 || frag.frameNumber > m_frame.frameNumber)) {
			m_pool.add(m_frame.frame, m_frame.frameRealSize);
			m_rcvMtx.lock();
			m_currentFrame = m_pool.next();
			m_rcvMtx.unlock();
		}

		int oldFrameSize = m_frame.frameSize;

		m_frame.fragmentNumber = 0;
		m_frame.frameNumber = frag.frameNumber;
		m_frame.fragmentSize = frag.fragmentSize;
		m_frame.frameFlags = frag.frameFlags;
		m_frame.fragmentPerFrame = frag.fragmentPerFrame;
		m_frame.frameSize = frag.fragmentPerFrame * frag.fragmentSize;
		m_frame.frameRealSize = m_frame.frameSize;

		if(oldFrameSize != m_frame.frameSize) {
			delete[] m_frame.frame;
			m_frame.frame = new UINT8[m_frame.frameSize];
			// reset the memory once and keep old fragment when 
			// the fragment of the new frame was not received
			memset(m_frame.frame, 0, m_frame.frameSize);
		}
		
		memset(m_frame.rcvWindow, 0, STREAM_FRAME_ACK);
	}

	if((int)((frag.fragmentNumber * m_frame.fragmentSize) + frag.fragmentSize) <= m_frame.frameSize) {
		memcpy(&m_frame.frame[frag.fragmentNumber * m_frame.fragmentSize], frag.fragmentData, frag.fragmentSize);
	}
	
	if(frag.fragmentNumber > m_frame.fragmentNumber) {
		if(frag.fragmentNumber + 1 == frag.fragmentPerFrame && (int)frag.fragmentSize < m_frame.fragmentSize) {
			m_frame.frameRealSize -= m_frame.fragmentSize - frag.fragmentSize;
		}
		m_frame.fragmentNumber = frag.fragmentNumber;
	}
	
	if(m_ackStreamFragment) {
		// fill the reception window and send it to ack the received fragment
		int bufferPosition = (int)floor(frag.fragmentNumber / 8 | 0);
		m_frame.rcvWindow[2 + bufferPosition] |= (UINT8)(1 << frag.fragmentNumber % 8);
		net::NetHelper::writeUInt16(frag.frameNumber, m_frame.rcvWindow, 0);
		return m_frame.rcvWindow;
	}
	return nullptr;
}
} // namespace net