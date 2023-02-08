/*!
* \file Service.h
* \brief Service interface.
* \author Ndongmo Silatsa
* \date 23-04-2022
* \version 2.0
*/

#pragma once

#include <string>

class IController;

/**
 * Define service basic methods.
 */
class Service
{
public:
    /**
     * Initialize the service and all required components.
     * \return 1 if the component has been correctly started, otherwise -1.
     */
	virtual int begin() = 0;

    /**
     * Start the service and all required components.
     */
	virtual void start() = 0;

    /**
	 * Stop the controller from outside.
	 */
    virtual void stop() { m_running = false; }

    /**
     * Stop the service and clean initialized components.
     * \return 1 if the component has been correctly started, otherwise -1.
     */
    virtual int end() = 0;

    /**
     * Get the service state.
     * @return true if the service is running, false otherwise. 
     */
    virtual bool isRunning() {
        return m_running;
    }

    /**
     * @brief Return the service's name
     * 
     * @return service's name
     */
    virtual const std::string& getName() const {
        return m_name;
    }
    
    /**
     * @brief Return the Service description
     * 
     * @return description
     */
    virtual std::string toString() { 
        return "No description"; 
    }

    /**
     * Set the service's controller.
     * @param controller new controller
     */
    void setController(IController* controller) {
        m_controller = controller;
    }

protected:
    /**
     * Run the service.
     */
    virtual void run() = 0;

    /**
     * Inform the error's listener of the given error.
     * @param error error code
     */
    void sendError(int error);

    /**
     * Running state.
     */
    bool m_running = false;

    /**
     * Service's name.
     */
    std::string m_name;

    /**
     * Pointer to service's controller.
     */
    IController* m_controller = nullptr;
};