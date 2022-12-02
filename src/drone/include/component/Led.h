/*!
* \file Led.h
* \brief Led management structures.
* \author Ndongmo Silatsa
* \date 09-11-2022
* \version 2.0
*/

#pragma once

#include <rpi_ws281x/ws2811.h>

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE              WS2811_STRIP_RGB
#define LED_COUNT               8
#define COLOR_COUNT             8

namespace component {

/**
 * Led action structure.
 */
struct LedAction {
    /* led id */
    unsigned int ledId = 0;
    /* led color index */
    unsigned int color = 0;
    /* action start delay */
    int start = 0;
    /* action delay */
    int delay = 0;
    /* Keep the delay value when repeating */
    int backup_delay = 0;
    /* nb of repeat (-1 = infinite) */
    int repeat = 0;
    /* alternate color */
    bool alternate = false;
    /* led on */
    bool on = false;
    /* one time turn on/off */
    bool once = false;
};

/**
 * Led color enum.
 */
enum LedColor {
    RED       = 0x00200000,  // red
    ORANGE    = 0x00201000,  // orange
    YELLOW    = 0x00202000,  // yellow
    GREEN     = 0x00002000,  // green
    LIGHTBLUE = 0x00002020,  // lightblue
    BLUE      = 0x00000020,  // blue
    PURPLE    = 0x00100010,  // purple
    PINK      = 0x00200010,  // pink
};

/**
 * Led color indexes.
 */
enum LedColorIndex {
    ID_RED       = 0,  // red
    ID_ORANGE    = 1,  // orange
    ID_YELLOW    = 2,  // yellow
    ID_GREEN     = 3,  // green
    ID_LIGHTBLUE = 4,  // lightblue
    ID_BLUE      = 5,  // blue
    ID_PURPLE    = 6,  // purple
    ID_PINK      = 7,  // pink
};

} // component