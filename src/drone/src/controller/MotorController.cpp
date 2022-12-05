#include "controller/MotorController.h"
#include "Constants.h"

#include <algorithm>

namespace controller {

int MotorController::begin() {
    return m_pca.open_pwm_pca9685();
}

int MotorController::end() {
    if(ComponentController::end() != 1) return -1;

    for(unsigned int i = 0; i < WHEEL_COUNT; i++) {
        m_pca.set_pwm_pca9685(i, 0, 0);
    }
    
    m_pca.close();

    return 1;
}

void MotorController::handleActions() {
    if(m_actions.size() > 0) {
        int laps = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_previousClock).count();
        for(auto& pair : m_actions) {
            pair.second.delay -= laps;
        }
    }

    m_mutex.lock();
    while(!m_queue.empty()) {
        auto& a = m_queue.front();
        m_actions[a.moveId] = a;
        m_queue.pop();
    }
    m_mutex.unlock();

    m_delay = 0;
    for(auto it = m_actions.begin(); it != m_actions.end();) {
        auto& p = *it;
        if(p.second.delay <= 0) {
            m_components[p.first] = 0;
            it = m_actions.erase(it);
        } else {
            m_components[p.first] = p.second.speed;
            m_delay = (m_delay == 0) ? p.second.delay : std::min(p.second.delay, m_delay);
            it++;
        }
    }

    for(auto& p : m_components) {
        m_pca.set_pwm_pca9685(p.first, p.second, 0);
    }

    m_previousClock = std::chrono::steady_clock::now();
}

void MotorController::move(DroneDir dir, DroneSpeed speed) {
    MotorAction tr {0, speed, MOTOR_MOVE_DELAY};
    MotorAction tl {0, speed, MOTOR_MOVE_DELAY};
    MotorAction br {0, speed, MOTOR_MOVE_DELAY};
    MotorAction bl {0, speed, MOTOR_MOVE_DELAY};

    if(dir == DroneDir::DIR_FORWARD) {
        tr.moveId = WHEEL_TR_FORWARD;
        tl.moveId = WHEEL_TL_FORWARD;
        br.moveId = WHEEL_BR_FORWARD;
        bl.moveId = WHEEL_BL_FORWARD;
    }
    else if(dir == DroneDir::DIR_BACKWARD) {
        tr.moveId = WHEEL_TR_BACKWARD;
        tl.moveId = WHEEL_TL_BACKWARD;
        br.moveId = WHEEL_BR_BACKWARD;
        bl.moveId = WHEEL_BL_BACKWARD;
    }
    else if(dir == DroneDir::DIR_LEFT) {
        tr.moveId = WHEEL_TR_FORWARD;
        tl.moveId = WHEEL_TL_BACKWARD;
        br.moveId = WHEEL_BR_FORWARD;
        bl.moveId = WHEEL_BL_BACKWARD;
    }
    else if(dir == DroneDir::DIR_RIGHT) {
        tr.moveId = WHEEL_TR_BACKWARD;
        tl.moveId = WHEEL_TL_FORWARD;
        br.moveId = WHEEL_BR_BACKWARD;
        bl.moveId = WHEEL_BL_FORWARD;
    }

    addActions(std::vector<MotorAction> {tr, tl, br, bl});
}

} // controller