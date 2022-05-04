/*!
* \file Types.h
* \brief Define needed data structure and enum.
* \author Ndongmo Silatsa
* \date 25-04-2022
* \version 2.0
*/

#pragma once

#define JOYSTICK_OFFSET 500

/*!
 * Define the type of handled controller event.
 */
enum CtrlEvent {
	GO_UP,
	GO_DOWN,
	GO_LEFT,
	GO_RIGHT,
	QUIT,
};

/*!
 * Event configuration structure.
 */
struct EventConfig {
	EventConfig() :keyID(0), joyID(0) {}
	EventConfig(unsigned int key, unsigned int joy) :keyID(key), joyID(joy) {}
    /*!
     * Event ID.
     */
	unsigned int keyID;
    /*!
     * Joystick ID related to the event ID.
     */
	unsigned int joyID;
};

/*!
 * Allow to pick what user wants to play with.
 */
enum class PlayWith { KEYBOARD, GAMEPAD };

/*!
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