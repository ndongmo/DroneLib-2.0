/*!
* \file EventHandler.h
* \brief Event handler class, it checks and stores events.
* \author Ndongmo Silatsa
* \date 24-10-2016
* \version 1.0
*/

#pragma once

#include <IEventHandler.h>

#include <string>
#include <unordered_map>
#include <SDL/SDL.h>

#define JOY_DEAD_ZONE 2000
#define JOY_AXE_MAX  32767
#define JOYSTICK_OFFSET 500
#define KEYS_CONFIG "keys.conf"

/**
 * Event configuration structure.
 */
struct EventConfig {
	EventConfig() :keyID(0), joyID(0) {}
	EventConfig(unsigned int key, unsigned int joy) :keyID(key), joyID(joy) {}
    /**
     * Event ID.
     */
	unsigned int keyID;
    /**
     * Joystick ID related to the event ID.
     */
	unsigned int joyID;
};

/**
 * Allow to pick what user wants to play with.
 */
enum class PlayWith { KEYBOARD, GAMEPAD };

/**
 * Joystick events (named according XBOX 360 Game pad).
 */
enum Joystick
{
	A = 10 + JOYSTICK_OFFSET, B = 11 + JOYSTICK_OFFSET, X = 12 + JOYSTICK_OFFSET, Y = 13 + JOYSTICK_OFFSET, HOME = 14 + JOYSTICK_OFFSET, LB = 8 + JOYSTICK_OFFSET, RB = 9 + JOYSTICK_OFFSET,
	L3 = 6 + JOYSTICK_OFFSET, R3 = 7 + JOYSTICK_OFFSET, SELECT = 5 + JOYSTICK_OFFSET, START = 4 + JOYSTICK_OFFSET, DPAD_UP = 0 + JOYSTICK_OFFSET, DPAD_DOWN = 1 + JOYSTICK_OFFSET,
	DPAD_LEFT = 2 + JOYSTICK_OFFSET, DPAD_RIGHT = 3 + JOYSTICK_OFFSET, LT = 15 + JOYSTICK_OFFSET, RT = 16 + JOYSTICK_OFFSET, AXE1_UP = 17 + JOYSTICK_OFFSET,
	AXE1_DOWN = 18 + JOYSTICK_OFFSET, AXE1_LEFT = 19 + JOYSTICK_OFFSET, AXE1_RIGHT = 20 + JOYSTICK_OFFSET, AXE2_UP = 21 + JOYSTICK_OFFSET, AXE2_DOWN = 22 + JOYSTICK_OFFSET,
	AXE2_LEFT = 23 + JOYSTICK_OFFSET, AXE2_RIGHT = 24 + JOYSTICK_OFFSET, NOTHING = 25 + JOYSTICK_OFFSET
};

/**
 * Handle controller (joystick and keyboar) events.
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
	 * Free SDL joystick resources.
	 */
	void destroy();

	/**
	 * Check controller state.
	 */
	void update();

	/**
	 * Update joystick axes.
	 * \param axe: concerned axe
	 * \param value : incline
	 * \param window : current SDL window
	 */
	void updateJoystickAxis(int axe, int value);
	/**
	 * Update joystick axes.
	 * \param value: wich value (UP, LEFT, ...)
	 */
	void updateJoystickHats(int value);

	/**
	 * Handle key pressed.
	 * \param keyID: ID of pressed key
	 */
	void pressKey(unsigned int keyID);

	/**
	 * Handle key released.
	 * \param keyID: ID of released key
	 */
	void releaseKey(unsigned int keyID);

	/**
	 * Handle key down action.
	 * \param keyID: key ID
	 */
	bool isKeyDown(unsigned int keyID);

	/**
	 * Check if key has just pressed.
	 * \param keyID: key ID
	 * \return true if key has just pressed
	 */
	bool isKeyPressed(unsigned int keyID);

	bool isEventDown(unsigned int eventID) override;

	bool isEventPressed(unsigned int eventID) override;

	/**
	 * Check if the event is registered.
	 * \param eventID: ID of event
	 * \return true if event exists, false otherwise
	 */
	bool eventExists(unsigned int eventID);

	/**
	 * Handle key released.
	 * \param keyID: ID of released key
	 */
	void releaseEvent(unsigned int eventID);

	/**
	 * Check if joystick is connected.
	 */
	bool isJoyConnect() { return m_joystick != nullptr; }

	/**
	 * Add or set event.
	 * \param eventID: event ID
	 * \param keyID: key ID
	 * \param joyID: joystick ID
	 */
	void addEvent(unsigned int eventID, unsigned int keyID, unsigned int joyID);

	std::string getMapping(unsigned int eventID) override;

	bool saveConfig();
	bool loadConfig();

	/* SETTERS */
	void setPlayWith(PlayWith util) { m_playWith = util; }

	/* GETTERS */
	PlayWith getPlayWith() { return m_playWith; }
	EventConfig getEventKey(unsigned int eventID) {
		return m_eventConfig[eventID];
	}

private:

	/* configuration of keyboard and gamepad */
	std::unordered_map<unsigned int, EventConfig> m_eventConfig;
	/* Map of current key ID */	
	std::unordered_map<unsigned int, bool> m_keyMap;
	/* util to play with */				
	PlayWith m_playWith = PlayWith::KEYBOARD;		
	/* Pointer to joystick structure */					
	SDL_Joystick* m_joystick = nullptr;									
};