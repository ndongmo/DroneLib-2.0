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
};