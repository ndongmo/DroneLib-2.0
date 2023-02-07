/*!
* \file Motor.h
* \brief Motor management structures.
* \author Ndongmo Silatsa
* \date 03-12-2022
* \version 2.0
*/

#pragma once

#include <cstdint>

#define MOTOR_FREQUENCY     50 // Motor frequency in Hz between 40 and 1600
#define WHEEL_COUNT         8  // Wheel actions count
#define SERVO_COUNT         2  // Number of servo motor
#define SERVO_MAX_ANGLE     180 // max rotation angle
#define SERVO_DEFAULT_ANGLE 90 // default rotation angle

namespace component {

// Registers/etc:
constexpr uint8_t MODE1              = 0x00;
constexpr uint8_t MODE2              = 0x01;
constexpr uint8_t SUBADR1            = 0x02;
constexpr uint8_t SUBADR2            = 0x03;
constexpr uint8_t SUBADR3            = 0x04;
constexpr uint8_t PRESCALE           = 0xFE;
constexpr uint8_t LED0_ON_L          = 0x06;
constexpr uint8_t LED0_ON_H          = 0x07;
constexpr uint8_t LED0_OFF_L         = 0x08;
constexpr uint8_t LED0_OFF_H         = 0x09;
constexpr uint8_t ALL_LED_ON_L       = 0xFA;
constexpr uint8_t ALL_LED_ON_H       = 0xFB;
constexpr uint8_t ALL_LED_OFF_L      = 0xFC;
constexpr uint8_t ALL_LED_OFF_H      = 0xFD;

// Bits:
constexpr uint8_t RESTART            = 0x80;
constexpr uint8_t SLEEP              = 0x10;
constexpr uint8_t ALLCALL            = 0x01;
constexpr uint8_t INVRT              = 0x10;
constexpr uint8_t OUTDRV             = 0x04;

/**
 * Motor action structure.
 */
struct MotorAction {
    /* Motor move index */
    unsigned int moveId;
    /* Motor speed */
    unsigned int speed;
    /* action delay */
    int delay;
};

/**
 * Servo action structure.
 */
struct ServoAction {
    /* Servo move index */
    unsigned int moveId;
    /* rotation angle increment */
    int angle;
};

/**
 * Motor move indexes.
 */
enum MotorMove {
    WHEEL_TL_FORWARD        = 0,  // wheel top left forward
    WHEEL_TL_BACKWARD       = 1,  // wheel top left backward
    WHEEL_BL_BACKWARD       = 2,  // wheel bottom left backward
    WHEEL_BL_FORWARD        = 3,  // wheel bottom left forward
    WHEEL_BR_FORWARD        = 4,  // wheel bottom right forward
    WHEEL_BR_BACKWARD       = 5,  // wheel bottom right backward
    WHEEL_TR_FORWARD        = 6,  // wheel top right forward
    WHEEL_TR_BACKWARD       = 7,  // wheel top right backward
};

/**
 * Servo rotation axes.
 */
enum ServoMove {
    SERVO_HORIZONTAL        = 0,  // servo handling horizontal rotation
    SERVO_VERTICAL          = 1,  // servo handling vertical rotation
};

} // component