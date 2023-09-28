/*!
* \file IEventHandler.h
* \brief Event handler interface.
* \author Ndongmo Silatsa
* \date 20/09/2023
* \version 2.0
*/

#pragma once

#include <string>

#define EVENT_COUNT 13 //nb of events

/**
 * Define the type of handled client event.
 */
enum ClientEvent {
	GO_UP = 0,
	GO_DOWN = 1,
	GO_LEFT = 2,
	GO_RIGHT = 3,
	GO_SPEED_1 = 4,
	GO_SPEED_2 = 5,
	CAM_UP = 6,
	CAM_DOWN = 7,
	CAM_LEFT = 8,
	CAM_RIGHT = 9,
	DISCOVER = 10,
	BUZZ = 11,
	QUIT = 12,
};

/**
 * Event handler interface.
 */
class IEventHandler
{
public:
    /**
	 * Handle event down action.
     * 
	 * \param eventID ID of the event
     * \return true if the key linked to the event is down, false otherwise.
	 */
	virtual bool isEventDown(unsigned int eventID) = 0;

	/**
	 * Check if event has just occured.
     * 
	 * \param eventID ID of the event
	 * \return true if the key linked to the event has been pressed, false otherwise.
	 */
	virtual bool isEventPressed(unsigned int eventID) = 0;

	/**
	 * Get the name of the trigger of the given event.
	 * 
	 * \param eventID: ID of event
	 * \return name of the key, joystick, touch or mouse button
	 */
	virtual std::string getMapping(unsigned int eventID) = 0;
};