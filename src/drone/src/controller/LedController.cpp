#include "controller/LedController.h"

#include <utils/Logger.h>

#include <rpi_ws281x/clk.h>
#include <rpi_ws281x/gpio.h>
#include <rpi_ws281x/dma.h>
#include <rpi_ws281x/pwm.h>

#include <algorithm>

namespace controller {

    const unsigned int LedController::m_colors[COLOR_COUNT] = {
        RED, ORANGE, YELLOW, GREEN, LIGHTBLUE, BLUE, PURPLE, PINK,
    };

    int LedController::begin() {
        ws2811_return_t ret;

        m_ledBuffer.freq = TARGET_FREQ;
        m_ledBuffer.dmanum = DMA;
        m_ledBuffer.channel [0] = {
            .gpionum = GPIO_PIN,
            .invert = 0,
            .count = LED_COUNT,
            .strip_type = STRIP_TYPE,
            .brightness = 255,
        };
        m_ledBuffer.channel [1] = {
            .gpionum = 0,
            .invert = 0,
            .count = 0,
            .brightness = 0,
        };
        
        if((ret = ws2811_init(&m_ledBuffer)) != WS2811_SUCCESS) {
            logE << "Led ws2811_init failed: " << ws2811_get_return_t_str(ret) << std::endl;
		    return -1;
        }
        return 1;
    }

    void LedController::start() {
        m_running = true;
        m_process = std::thread([this]{run(); });
    }

    int LedController::end() {
        m_mutex.lock();
        m_running = false;
        m_mutex.unlock();
        m_cv.notify_all();

        if(m_process.joinable()) {
            m_process.join();
        }

        for(unsigned int i = 0; i < LED_COUNT; i++) {
            m_ledBuffer.channel[0].leds[i] = 0;
        }
        ws2811_return_t ret;
        if ((ret = ws2811_render(&m_ledBuffer)) != WS2811_SUCCESS) {
            logE << "Led ws2811_render failed when clearing: " << ws2811_get_return_t_str(ret) << std::endl;
        }

        ws2811_fini(&m_ledBuffer);

        return 1;
    }

    bool LedController::isRunning() {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_running;
    }

    void LedController::run() {
        while (isRunning()) {
            handleActions();
            waitNextAction();
        }
    }

    bool LedController::isOn(unsigned int ledIndex) {
        if(ledIndex < LED_COUNT && m_leds.find(ledIndex) != m_leds.end()) {
            return m_leds[ledIndex] != 0;
        }
        return false;
    }

    void LedController::turn(unsigned int ledIndex, bool state, unsigned int colorIndex) {
        LedAction a; a.ledId = ledIndex; a.color = colorIndex; a.on = state;
        addAction(a);
    }

    void LedController::turnAll(bool state, unsigned int colorIndex) {
        std::vector<LedAction> actions;
        for(unsigned int i = 0; i < LED_COUNT; i++) {
            LedAction a; a.ledId = i; a.color = colorIndex; a.on = state;
            actions.push_back(a);
        }
        addActions(actions);
    }

    void LedController::playWheel(unsigned int laps) {
        std::vector<LedAction> actions;
        for(unsigned int i = 0; i < LED_COUNT; i++) {
            LedAction a; a.ledId = i; a.color = i; a.on = true; a.delay = laps; 
            a.alternate = true; a.repeat = -1;
            actions.push_back(a);
        }
        addActions(actions);
    }

    void LedController::addAction(const LedAction& action) {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_queue.push(action);
        m_cv.notify_all();
    }

    void LedController::addActions(const std::vector<LedAction>& actions) {
        std::lock_guard<std::mutex> guard(m_mutex);
        for(const auto& a : actions) {
            m_queue.push(a);
        }
        m_cv.notify_all();
    }

    void LedController::updateLed(const LedAction& a) {
        m_leds[a.ledId] = a.on ? m_colors[a.color] : 0;
        m_ledBuffer.channel[0].leds[a.ledId] = m_leds[a.ledId];
    }

    void LedController::handleActions() {
        if(m_actions.size() > 0) {
            int laps = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - m_previousClock).count();
            for(auto& pair : m_actions) {
                if(pair.second.start > 0) {
                    if(pair.second.start >= laps) {
                        pair.second.start -= laps;
                    } else {
                        pair.second.delay -= laps - pair.second.start;
                        pair.second.start = 0;
                    }
                } else {
                    pair.second.delay -= laps;
                }
            }
        }

        m_mutex.lock();
        while(!m_queue.empty()) {
            auto& a = m_queue.front();
            m_actions[a.ledId] = a;
            m_actions[a.ledId].backup_delay = m_actions[a.ledId].delay;
            m_actions[a.ledId].once = a.delay <= 0;
            m_queue.pop();
        }
        m_mutex.unlock();

        m_delay = 0;
        for(auto it = m_actions.begin(); it != m_actions.end();) {
            auto& p = *it;
            if(p.second.start <= 0 && p.second.delay <= 0) {
                if(p.second.repeat > 0 || p.second.repeat < 0) {
                    if(p.second.alternate) {
                        p.second.color++;
                        p.second.color = p.second.color % COLOR_COUNT;
                        if(p.second.repeat > 0) {
                            p.second.repeat--;
                        }
                    } else {
                        p.second.on = !p.second.on;
                        if(p.second.repeat > 0 && p.second.on) {
                            p.second.repeat--;
                        }
                    }
                    p.second.delay = p.second.backup_delay;
                } else {
                    if(!p.second.once) {
                        p.second.on = !p.second.on;
                    }
                    updateLed(p.second);
                    it = m_actions.erase(it);
                    continue;
                }
            }
            if(m_delay == 0) {
                m_delay = (p.second.start > 0) ? p.second.start : p.second.delay;
            } else {
                m_delay = (p.second.start > 0) ? std::min(p.second.start, m_delay) 
                    : std::min(p.second.delay, m_delay);
            }
            it++;
        }

        for(auto& p : m_actions) {
            if(p.second.start <= 0) {
                updateLed(p.second);
            }
        }

        ws2811_return_t ret;
        if ((ret = ws2811_render(&m_ledBuffer)) != WS2811_SUCCESS) {
            logE << "Led ws2811_render failed: " << ws2811_get_return_t_str(ret) << std::endl;
        }

        m_previousClock = std::chrono::steady_clock::now();
    }

    void LedController::waitNextAction() {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_delay > 0) {
            m_cv.wait_for(lock, std::chrono::milliseconds(m_delay), [this] { 
                return !m_queue.empty() || !m_running;
            });
        } else {
            m_cv.wait(lock, [this] { return !m_queue.empty() || !m_running; });
        }
    }

} // controller