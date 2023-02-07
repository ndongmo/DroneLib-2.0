#include "controller/ServoController.h"

#include <utils/Constants.h>

#include <algorithm>

namespace controller {

int ServoController::begin() {
    int ret = 1;
#ifdef __arm__
    ret = m_pca.open_pwm_pca9685();
#endif
    return ret;
}

int ServoController::end() {
    if(ComponentController::end() != 1) return -1;

    init();
    
#ifdef __arm__
    m_pca.close();
#endif

    return 1;
}

void ServoController::init() {
    for(unsigned int i = 0; i < SERVO_COUNT; i++) {
        setServoAngle(i, SERVO_DEFAULT_ANGLE);
    }
}

void ServoController::start() {
    init();
    ComponentController::start();
}

void ServoController::handleActions() {
    m_mutex.lock();
    while(!m_queue.empty()) {
        auto& a = m_queue.front();
        m_actions[a.moveId] = a;
        m_queue.pop();
    }
    m_mutex.unlock();

    for(auto &a : m_actions) {
        if(m_components.find(a.first) != m_components.end()) {
            int angle = m_components[a.first] + a.second.angle;
            if(angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
            else if(angle <= 0) angle = 0;

            setServoAngle(a.first, angle);
        }
    }
    m_actions.clear();
}

void ServoController::rotate(DroneCamera axe, int angle) {
    ServoAction action {0, angle};

    if(axe == DroneCamera::CAMERA_X_AXE) {
        action.moveId = SERVO_HORIZONTAL;
    } else {
        action.moveId = SERVO_VERTICAL;
    }

    addAction(action);
}

void ServoController::setServoAngle(unsigned int index, unsigned int angle) {
    int pulse;

    if(m_components[index] != angle) {
        m_components[index] = angle;

        if(index == SERVO_HORIZONTAL) {
            pulse = 2500 - (int)(angle / 0.09f);
        } else {
            pulse = 500 + (int)(angle / 0.09f);
        }
        pulse = pulse * 4096 / 20000;
        index = index + WHEEL_COUNT;
#ifdef __arm__
        m_pca.set_pwm_pca9685(index, 0, pulse);
#endif
    }
}

} // controller