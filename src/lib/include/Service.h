/*!
* \file Service.h
* \brief Service interface.
* \author Ndongmo Silatsa
* \date 23-04-2022
* \version 2.0
*/

#pragma once

#include <thread>

/*!
 * Define service basic methods.
 */
class Service
{
public:
    /*!
     * Initialize the service and all required components.
     * \return 1 if the component has been correctly started, otherwise -1.
     */
	virtual int begin() = 0;

    /*!
     * Start the service and all required components.
     */
	virtual void start() = 0;

    /*!
     * Stop the service and clean initialized components.
     * \return 1 if the component has been correctly started, otherwise -1.
     */
    virtual int end() = 0;

    /*!
     * Get the service state.
     * @return true if the service is running, false otherwise. 
     */
    virtual bool isRunning() {
        return m_running;
    }

protected:
    /*!
     * Run the service.
     */
    virtual void run() = 0;

    /*!
     * Running state.
     */
    bool m_running = false;
};