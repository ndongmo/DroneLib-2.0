/*!
* \file LedController.h
* \brief Led generic controller class.
* \author Ndongmo Silatsa
* \date 21-09-2022
* \version 2.0
*/

#pragma once

#include "component/Led.h"
#include "ComponentController.h"

#include <utils/Structs.h>

#include <rpi_ws281x/ws2811.h>

#define STATE_LEDS 2    // number of leds for state highlighting

namespace controller {

using namespace component;

/**
 * Led controller class in charge of leds manipulation.
 */
class LedController : public ComponentController<LedAction, LED_COUNT>
{
public:    
    int begin() override;
    int end() override;

    /**
     * Turn the led on/off.
     * @param ledIndex led ID or index
     * @param state on/off
     * @param colorIndex color index unused when turn off
     */
    void turn(unsigned int ledIndex, bool state, unsigned int colorIndex = LedColorIndex::ID_GREEN);

    /**
     * Turn all leds on/off.
     * @param state on/off
     * @param colorIndex color index unused when turn off
     */
    void turnAll(bool state, unsigned int colorIndex = LedColorIndex::ID_GREEN);

    /**
     * Play a wheel simple animation.
     * @param laps laps duration in ms
     */
    void playWheel(unsigned int laps);

    /**
     * Play leds animation regarding the given state.
     * @param state app curent state
     */
    void play(utils::AppState state);

protected:
    void handleActions() override;

    /**
     * Update the led on/off with the given led action.
     * @param a led action
     */
    void updateLed(const LedAction& a);

    /** Leds buffer value */
    ws2811_t m_ledBuffer = {};
    /** state leds */
    static const unsigned int m_state_leds[STATE_LEDS];
    /** colors array */
    static const unsigned int m_colors[COLOR_COUNT];
};
} // controller