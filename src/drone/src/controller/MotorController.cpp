#include "controller/MotorController.h"

#include <utils/Constants.h>

#include <algorithm>

namespace controller {

MotorController::MotorController() {
    m_name = "WheelsService";
}
int MotorController::begin() {
    int ret = 1;
#ifdef __arm__
    ret = m_pca.open_pwm_pca9685();
#endif
    return ret;
}

int MotorController::end() {
    int ret = ComponentController::end();

#ifdef __arm__
    for(unsigned int i = 0; i < WHEEL_COUNT; i++) {
        m_pca.set_pwm_pca9685(i, 0, 0);
    }
    m_pca.close();
#endif

    return ret;
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

        int toRemove = (a.moveId % 2 == 0) ? a.moveId + 1 : a.moveId - 1;
        if(m_actions.find(toRemove) != m_actions.end()) {
            m_actions.erase(toRemove);
            m_components[toRemove] = 0;
        }
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

#ifdef __arm__
    for(auto& p : m_components) {
        m_pca.set_pwm_pca9685(p.first, p.second, 0);
    }
#endif

    m_previousClock = std::chrono::steady_clock::now();
}

void MotorController::move(DroneDir dir, DroneSpeed speed) {
    MotorAction tr {0, speed, MOTORS_MOVE_LAPS};
    MotorAction tl {0, speed, MOTORS_MOVE_LAPS};
    MotorAction br {0, speed, MOTORS_MOVE_LAPS};
    MotorAction bl {0, speed, MOTORS_MOVE_LAPS};

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