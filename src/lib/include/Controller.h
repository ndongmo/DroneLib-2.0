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
	 * @return -1 when the initialization failed, otherwise 1
	 */
	int beginService(Service& service);

	/**
	 * @brief Start the given service when it is activated.
	 * 
	 * @param service service to start
	 */
	void startService(Service& service);

	/**
	 * @brief Stop the given service when it is activated.
	 * 
	 * @param service service to stop
	 */
	void stopService(Service& service);

	/**
	 * @brief End the given service when it is activated.
	 * 
	 * @param service service to end
	 * @return -1 when the ending failed, otherwise 1
	 */
	int endService(Service& service);
};