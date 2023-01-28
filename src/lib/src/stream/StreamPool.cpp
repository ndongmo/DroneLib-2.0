#include "stream/StreamPool.h"

#include <string.h>

namespace stream {

void StreamPool::add(const UINT8 *data, int size) {
	if(m_size[m_writeIndex] != size) {
		if(m_size[m_writeIndex] > 0) {
			delete[] m_data[m_writeIndex];
		}
		m_size[m_writeIndex] = size;
		m_data[m_writeIndex] = new UINT8[size];
	}

	memcpy(m_data[m_writeIndex], data, size);

	if (++m_writeIndex >= POOL_SIZE) m_writeIndex = 0;
}

int StreamPool::next() {
	if (m_readIndex == m_writeIndex) return -1;

	int readIndex = m_readIndex;
	
	if (++m_readIndex >= POOL_SIZE) m_readIndex = 0;

	return readIndex;
}

UINT8* StreamPool::getData(int readIndex) const {
	if(readIndex >= 0 && m_readIndex < POOL_SIZE) {
		return m_data[readIndex];
	}
	return nullptr;
}

int StreamPool::getSize(int readIndex) const {
	if(readIndex >= 0 && m_readIndex < POOL_SIZE) {
		return m_size[readIndex];
	}
	return 0;
}

StreamPool::~StreamPool() {
	for (int i = 0; i < POOL_SIZE; i++)
		delete[] m_data[i];
}
} // namespace stream