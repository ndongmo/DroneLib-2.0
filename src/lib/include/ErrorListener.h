/*!
* \file ErrorListener.h
* \brief Error handler class.
* \author Ndongmo Silatsa
* \date 20-11-2022
* \version 2.0
*/

#pragma once

#include <mutex>
#include <queue>
#include <unordered_map>

/*!
 * Handle incoming error in a queue.
 */
class ErrorListener
{
public:
    /*!
     * Add error to the error's queue.
     * @param error error code
     */
	void add(int error);

protected:
    /*!
     * Handle errors in the queue.
     */
    void handleErrors();

    /*!
     * Handle the given error.
     * @param error error code
     */
    virtual void handleError(int error) = 0;

    /* Mutex for error's queue synchronization */
    std::mutex m_error_mutex;
    /* Error's queue */
    std::queue<int> m_error_queue;
    /* Error occurences */
    std::unordered_map<int, unsigned int> m_error_occurences;
};