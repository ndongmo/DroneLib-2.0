/*!
* \file PCHelper.h
* \brief Helper file for providing enums, structure, etc.
* \author Ndongmo Silatsa
* \date 24-11-2022
* \version 2.0
*/

#pragma once

/*!
 * PC app different states.
 */
enum PCState {
    PC_INIT         = 0,
    PC_DISCOVERING  = 1,
    PC_RUNNING      = 2,
    PC_CLOSING      = 3,
    PC_ERROR        = 4,
};