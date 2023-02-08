/*!
* \file Controller.h
* \brief Main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "Service.h"
#include "IController.h"
#include "utils/Structs.h"

#include <mutex>

/**
 * Controller main class in charge of starting and stoping all other services.
 */
class Controller : public Service, public IController
{
public:
    /**
	 * Starts a handshake connection and initializes required variables.
	 * \return -1 when the communication failed, otherwise 1.
	 */
	virtual int discovery() = 0;

	/**
     * Initialize all required config variables.
     */
    virtual void initConfigs();

protected:
	/**
	 * @brief Initialize the given service when it is activated.
	 * 
	 * @param service service to initialize
	 * @param activeID active ID of the service
	 * @return -1 when the initialization failed, otherwise 1
	 */
	int beginService(Service& service, const char* activeID);

	/**
	 * @brief Start the given service when it is activated.
	 * 
	 * @param service service to start
	 * @param activeID active ID of the service
	 */
	void startService(Service& service, const char* activeID);

	/**
	 * @brief Stop the given service when it is activated.
	 * 
	 * @param service service to stop
	 * @param activeID active ID of the service
	 */
	void stopService(Service& service, const char* activeID);

	/**
	 * @brief End the given service when it is activated.
	 * 
	 * @param service service to end
	 * @param activeID active ID of the service
	 * @return -1 when the ending failed, otherwise 1
	 */
	int endService(Service& service, const char* activeID);
};