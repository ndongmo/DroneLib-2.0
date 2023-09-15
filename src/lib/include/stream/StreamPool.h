/*!
* \file StreamPool.h
* \brief Pool stream data manager class.
* \author Ndongmo Silatsa Fabrice
* \date 12-03-2019
* \version 2.0
*/

#pragma once

#include <mutex>

#ifdef _WIN32 
#include <WS2tcpip.h>
#else
#include <sys/types.h>

typedef uint8_t UINT8;
#endif

#define POOL_SIZE 64

namespace stream
{

/**
 * Pool class for stream data.
 */
class StreamPool
{
public:
	~StreamPool();

	/**
	 * Initialize a data buffer in the pool with the given size and copy the given data
	 * in this buffer.
	 * \param data data to copy
	 * \param size data size
	 */
	void add(const UINT8* data, int size);

	/**
	 * Get the current read index and increament it afterwards.
	 * \return the read index
	 */
	int next();
	
	/**
	* Retrieves the current stream data (relying on the readindex cursor).
	* \param readIndex data index
	* \return stream data at the given index
	*/
	UINT8* getData(int readIndex) const;

	/**
	* Retrieves the current stream size (relying on the readindex cursor).
	* \param readIndex size index
	* \return stream size at the given index
	*/
	int getSize(int readIndex) const;

private:
	/** Pool cursor for reading data */
	int m_readIndex = 0;
    /** Pool cursor for adding data */
	int m_writeIndex = 0;
	/** Pool size buffer */
	int m_size[POOL_SIZE] = {0};
	/** Pool data buffer */
	UINT8* m_data[POOL_SIZE] = {0};
	
};
} // namespace stream