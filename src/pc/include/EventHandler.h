/*!
* \file EventHandler.h
* Event handler class, it checks and stores events.
* \author Ndongmo Silatsa
* \date 24-10-2016
* \version 1.0
*/

#pragma once

#include "Types.h"

#include <string>
#include <unordered_map>
#include <SDL/SDL.h>

#define JOY_DEAD_ZONE 2000
#define JOY_AXE_MAX  32767
#define KEYS_CONFIG "keys.conf"

/*!
 * Handle controller (joystick and keyboar) events.
 * Also load and save controller configuration.
 */
class EventHandler
{
public:
	/*!
	 * Initialize joystick SDL stuffs.
	 */
	void init();

	/*!
	 * Free SDL joystick resources.
	 */
	void destroy();

	/*!
	 * Check controller state.
	 */
	void update();

	/*!
	 * Update joystick axes.
	 * \param axe: concerned axe
	 * \param value : incline
	 * \param window : current SDL window
	 */
	void updateJoystickAxis(int axe, int value);
	/*!
	 * Update joystick axes.
	 * \param value: wich value (UP, LEFT, ...)
	 */
	void updateJoystickHats(int value);

	/*!
	 * Handle key pressed.
	 * \param keyID: ID of pressed key
	 */
	void pressKey(unsigned int keyID);

	/*!
	 * Handle key released.
	 * \param keyID: ID of released key
	 */
	void releaseKey(unsigned int keyID);

	/*!
	 * Handle key down action.
	 * \param keyID: key ID
	 */
	bool isKeyDown(unsigned int keyID);

	/*!
	 * Check if key has just pressed.
	 * \param keyID: key ID
	 * \return true if key has just pressed
	 */
	bool isKeyPressed(unsigned int keyID);

	/*!
	 * Handle event down action.
	 * \param eventID: ID of event
	 */
	bool isEventDown(unsigned int eventID);

	/*!
	 * Check if event has just occured.
	 * \param eventID: ID of event
	 * \return true if event has just occured
	 */
	bool isEventPressed(unsigned int eventID);

	/*!
	 * Check if the event is registered.
	 * \param eventID: ID of event
	 * \return true if event exists, false otherwise
	 */
	bool eventExists(unsigned int eventID);

	/*!
	 * Handle key released.
	 * \param keyID: ID of released key
	 */
	void releaseEvent(unsigned int eventID);

	/*!
	 * Check if joystick is connected.
	 */
	bool isJoyConnect() { return m_joystick != nullptr; }

	/*!
	 * Add or set event.
	 * \param eventID: event ID
	 * \param keyID: key ID
	 * \param joyID: joystick ID
	 */
	void addEvent(unsigned int eventID, unsigned int keyID, unsigned int joyID);

	/*!
	 * Update mapping with pressed key.
	 * \param eventID: ID of event
	 */
	void updateMapping(unsigned int eventID, PlayWith util);

	/*!
	 * Get the key's name.
	 * \param eventID: ID of event
	 * \return name of key, joystick or mouse button
	 */
	std::string getMapping(unsigned int eventID, PlayWith util);

	/*!
	 * Get the key's name.
	 * \param eventID: ID of event
	 * \return name of key, joystick or mouse button
	 */
	std::string getMapping(unsigned int eventID);

	void updateConfig();
	void saveConfig();
	bool loadConfig();
	void clearConfig();

	/* SETTERS */
	void setPlayWith(PlayWith util) { m_playWith = util; }

	/* GETTERS */
	PlayWith getPlayWith() { return m_playWith; }
	EventConfig getEventKey(unsigned int eventID) {
		return m_eventConfig[eventID];
	}

private:
	/*!
	 * Save event keys config file.
	 * \return true if the config file has been saved, false otherwise
	 */
	bool saveConfigFile();

	/*!
	 * Retrieve the controller key name corresponding to the given event ID.
	 * \param eventID: ID of event
	 * \param util: controller type
	 * \param map: database map to search into
	 * \return controller key name
	 */
	std::string getMapping(unsigned int eventID, PlayWith util, std::unordered_map<unsigned int, EventConfig>& map);

	/* configuration of keyboard and gamepad */
	std::unordered_map<unsigned int, EventConfig> m_eventConfig;
	/* Allow user to configure keyboard or gamepad */
	std::unordered_map<unsigned int, EventConfig> m_eventConfigTemp;
	/* Map of current key ID */	
	std::unordered_map<unsigned int, bool> m_keyMap;
	/* util to play with */				
	PlayWith m_playWith = PlayWith::KEYBOARD;		
	/* Pointer to joystick structure */					
	SDL_Joystick* m_joystick = nullptr;									
};