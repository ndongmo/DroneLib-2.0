/*!
* \file Controller.h
* \brief Main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "Service.h"

/*!
 * Controller main class in charge of starting and stoping all other services.
 */
class Controller : public Service
{
public:
    /*!
	 * Starts a handshake connection and initializes required variables.
	 * \return -1 when the communication failed, otherwise 1.
	 */
	virtual int discovery() = 0;

    /*!
     * Get the max fragment size.
     * \return max fragment size
     */
    int getMaxFragementSize() const {
        return m_maxFragmentSize;
    }

	/*!
     * Get the max fragment number.
     * \return max fragment number
     */
    int getMaxFragementNumber() const {
        return m_maxFragmentNumber;
    }

protected:
    /*! Max fragment size */
	int m_maxFragmentSize;
	/*! Max fragment number */
	int m_maxFragmentNumber;
};