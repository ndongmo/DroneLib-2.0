#include "stream/StreamPool.h"

#include <string.h>

namespace stream {

void StreamPool::add(const UINT8 *data, int size) {
	m_data[m_writeIndex] = new UINT8[size];
	m_size[m_writeIndex] = size;

	memcpy(m_data[m_writeIndex], data, size);

	if (++m_writeIndex >= POOL_SIZE) m_writeIndex = 0;
}

int StreamPool::next() {

	if (m_readIndex == m_writeIndex) return -1;

	int readIndex = m_readIndex;
	
	if (++m_readIndex >= POOL_SIZE) m_readIndex = 0;

	return readIndex;
}

const UINT8* StreamPool::getData(int readIndex) const {
	return m_data[readIndex];
}

int StreamPool::getSize(int readIndex) const {
	return m_size[readIndex];
}

StreamPool::~StreamPool() {
	for (int i = 0; i < POOL_SIZE; i++)
		delete[] m_data[i];
}
} // namespace stream