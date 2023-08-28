/*!
* \file Buzzer.h
* \brief Buzzer management structures.
* \author Ndongmo Silatsa
* \date 28-08-2023
* \version 2.0
*/

#pragma once

#define BUZZER_PIN         0 // wiringPi pin 0 is BCM_GPIO 17
#define BUZZER_ID          0
#define BUZZER_COUNT       1

namespace component {

/**
 * Buzzer action structure.
 */
struct BuzzerAction {
    /* action start delay */
    int start = 0;
    /* action delay */
    int delay = 0;
    /* Keep the delay value when repeating */
    int backup_delay = 0;
    /* nb of repeat (-1 = infinite) */
    int repeat = 0;
    /* buzzer on */
    bool on = false;
    /* one time turn on/off */
    bool once = false;
};

} // component