/*!
* \file LedController.h
* \brief Led generic controller class.
* \author Ndongmo Silatsa
* \date 21-09-2022
* \version 2.0
*/

#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <condition_variable>

#include <rpi_ws281x/ws2811.h>

#include <Service.h>

#include "component/Led.h"

namespace controller {

using namespace component;

/*!
 * Led controller class in charge of leds manipulation.
 */
class LedController : Service
{
public:    
    int begin() override;
    void start() override;
    int end() override;
    bool isRunning() override;

    /*!
     * Turn the led on/off.
     * @param ledIndex led ID or index
     * @param state on/off
     * @param colorIndex color index unused when turn off
     */
    void turn(unsigned int ledIndex, bool state, unsigned int colorIndex = LedColorIndex::ID_GREEN);

    /*!
     * Turn all leds on/off.
     * @param state on/off
     * @param colorIndex color index unused when turn off
     */
    void turnAll(bool state, unsigned int colorIndex = LedColorIndex::ID_GREEN);

    /*!
     * Play a wheel simple animation.
     * @param laps laps duration in ms
     */
    void playWheel(unsigned int laps);

    /*!
     * Get the led state.
     * @param ledIndex index of the led
     * \return true when the led is on, false otherwise
     */
    bool isOn(unsigned int ledIndex);

    /*!
     * Add led action in the queue.
     * @param action action to add
     */
    void addAction(const LedAction& action);

    /*!
     * Add led actions in the queue.
     * @param action action to add
     */
    void addActions(const std::vector<LedAction>& actions);

protected:
    void run() override;

    /* Handle action in the queue or/and in the list of actions */
    void handleActions();

    /* Wait for the next action */
    void waitNextAction();

    /*!
     * Update the led on/off with the given led action.
     * @param a led action
     */
    void updateLed(const LedAction& a);

    /* Leds buffer value */
    ws2811_t m_ledBuffer = {}; //initialize!!!
    /* Mutex for queue synchronization */
    std::mutex m_mutex;
    /* Condition variable for controlling access to the mutex */
    std::condition_variable m_cv;
    /* Led handling thread */
    std::thread m_process;

    /* Next action delay in millisecond */
    int m_delay = 0;
    /* Elapsed time since the last loop */
    std::chrono::steady_clock::time_point m_previousClock;
    /* colors array */
    static const unsigned int m_colors[COLOR_COUNT];

    /* Queue of led actions*/
    std::queue<LedAction> m_queue;
    /* Current executing led actions */
    std::unordered_map<unsigned int, LedAction> m_actions;
    /* Led colors map */
    std::unordered_map<unsigned int, unsigned int> m_leds;
};
} // controller