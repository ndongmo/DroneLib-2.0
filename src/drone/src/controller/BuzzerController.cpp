#include "controller/BuzzerController.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Constants.h>

#include <wiringPi/wiringPi.h>

#include <chrono>
#include <algorithm>

using namespace utils;

namespace controller {

BuzzerController::BuzzerController() {
    m_name = "BuzzerService";
}

int BuzzerController::begin() {
#ifdef __arm__  
    wiringPiSetup();
    pinMode(BUZZER_PIN, OUTPUT);
#endif
    return 1;
}

int BuzzerController::end() {
    int ret1 = ComponentController::end();
#ifdef __arm__  
    digitalWrite(BUZZER_PIN, LOW);	// Off
#endif
    return ret1;
}

void BuzzerController::turn(bool state) {
    BuzzerAction a; a.delay = BUZZER_LAPS; a.on = state;
    addAction(a);
}

void BuzzerController::play(utils::AppState state) {
    if(state == APP_DISCOVERING) {
        BuzzerAction a; a.delay = DISCOVERY_LAPS; a.on = true;  a.repeat = -1;
        addAction(a);
    }
    else if(state == APP_RUNNING) {
        BuzzerAction a; a.on = false;
        addAction(a);
    }
    else if(state == APP_ERROR) {
        BuzzerAction a; a.delay = DISCOVERY_LAPS; a.on = true;  a.repeat = -1;
        addAction(a);
    } else {
        turn(false);
    }
}

void BuzzerController::updateBuzzer(const BuzzerAction& a) {
    m_components[BUZZER_ID] = a.on ? 1 : 0;
#ifdef __arm__
    if(a.on) {
        digitalWrite(BUZZER_PIN, HIGH);	// On
    } else {
        digitalWrite(BUZZER_PIN, LOW);	// Off
    }
#endif
}

void BuzzerController::handleActions() {
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
        m_actions[BUZZER_ID] = a;
        m_actions[BUZZER_ID].backup_delay = m_actions[BUZZER_ID].delay;
        m_actions[BUZZER_ID].once = a.delay <= 0;
        m_queue.pop();
    }
    m_mutex.unlock();

    m_delay = 0;
    for(auto it = m_actions.begin(); it != m_actions.end();) {
        auto& p = *it;
        if(p.second.start <= 0 && p.second.delay <= 0) {
            if(p.second.repeat > 0 || p.second.repeat < 0) {
                p.second.on = !p.second.on;
                if(p.second.repeat > 0 && p.second.on) {
                    p.second.repeat--;
                }
                p.second.delay = p.second.backup_delay;
            } else {
                if(!p.second.once) {
                    p.second.on = !p.second.on;
                }
                updateBuzzer(p.second);
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
            updateBuzzer(p.second);
        }
    }

    m_previousClock = std::chrono::steady_clock::now();
}

} // controller