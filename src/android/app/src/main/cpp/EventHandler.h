/*!
* \file EventHandler.h
* \brief Event handler class, it checks and stores events.
* \author Ndongmo Silatsa
* \date 24-9-2023
* \version 1.0
*/

#pragma once

#include <IEventHandler.h>

#include <string>
#include <unordered_map>

/**
 * Handle Android touch events.
 * Also load and save controller configuration.
 */
class EventHandler : public IEventHandler
{
public:
	/**
	 * Initialize joystick SDL stuffs.
	 */
	void init();

	/**
	 * Handle event pressed.
	 * \param eventID: ID of pressed event
	 */
	void pressEvent(unsigned int eventID);

	bool isEventDown(unsigned int eventID) override;

	bool isEventPressed(unsigned int eventID) override;

	/**
	 * Handle event released.
	 * \param eventID: ID of released event
	 */
	void releaseEvent(unsigned int eventID);

	std::string getMapping(unsigned int eventID) override;

private:
	/* Map of current events ID */	
	std::unordered_map<unsigned int, bool> m_eventMap;				
};