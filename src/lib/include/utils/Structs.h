/*!
* \file Structs.h
* \brief Helper file for providing enums, structure, etc.
* \author Ndongmo Silatsa
* \date 24-11-2022
* \version 2.0
*/

#pragma once

namespace utils
{
/**
 * App different states.
 */
enum AppState {
    APP_INIT         = 0,
    APP_DISCOVERING  = 1,
    APP_RUNNING      = 2,
    APP_CLOSING      = 3,
    APP_ERROR        = 4,
};

/**
 * Drone moving speeds.
 */
enum DroneSpeed {
    SPEED_LOW       = 2048,
    SPEED_HIGH      = 1024,
    SPEED_MAX       = 4096,
};

/**
 * Drone moving direction.
 */
enum DroneDir {
    DIR_FORWARD     = 0,
    DIR_BACKWARD    = 1,
    DIR_LEFT        = 2,
    DIR_RIGHT       = 3,
};
}