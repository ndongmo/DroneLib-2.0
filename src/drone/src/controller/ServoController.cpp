#include "controller/ServoController.h"

#include <utils/Constants.h>
#include <utils/Logger.h>

#include <algorithm>

namespace controller {

ServoController::ServoController() {
    m_name = "CameraRotationService";
}

int ServoController::begin() {
    int ret = 1;
#ifdef __arm__
    ret = m_pca.open_pwm_pca9685();
#endif
    return ret;
}

int ServoController::end() {
    int ret = ComponentController::end();

    init();
    
#ifdef __arm__
    m_pca.close();
#endif

    return ret;
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
    if(m_actions.size() > 0) {
        m_delay -= std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::steady_clock::now() - m_previousClock).count();
    }

    m_mutex.lock();
    while(!m_queue.empty()) {
        auto& a = m_queue.front();
        m_actions[a.moveId] = a;
        m_queue.pop();
    }
    m_mutex.unlock();

    if(m_delay <= 0) {
        for(auto it = m_actions.begin(); it != m_actions.end();) {
            if(m_components.find(it->first) != m_components.end()) {
                int angle = (it->second.angle < 0) ? -SERVOS_MOVE_APL : SERVOS_MOVE_APL;
                it->second.angle -= angle;
                angle = m_components[it->first] + angle;

                if(it->first == SERVO_HORIZONTAL) {
                    if(angle > SERVO_MAX_ANGLE_X)       angle = SERVO_MAX_ANGLE_X;
                    else if(angle < SERVO_MIN_ANGLE_X)  angle = SERVO_MIN_ANGLE_X;
                } else {
                    if(angle > SERVO_MAX_ANGLE_Y)       angle = SERVO_MAX_ANGLE_Y;
                    else if(angle < SERVO_MIN_ANGLE_Y)  angle = SERVO_MIN_ANGLE_Y;
                }
                
                setServoAngle(it->first, angle);

                if(abs(it->second.angle) < SERVOS_MOVE_APL) {
                    it = m_actions.erase(it);
                } else {
                    it++;
                }
            }
        }
        m_delay = (m_actions.size() > 0) ? SERVOS_MOVE_LAPS : 0;
    }
    m_previousClock = std::chrono::steady_clock::now();
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