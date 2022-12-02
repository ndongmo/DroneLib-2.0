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
}