#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include <utils/Constants.h>

#include <component/Motor.h>
#include <controller/ServoController.h>

using namespace controller;
using namespace component;
using namespace utils;

class ServoControllerTest : public ::testing::Test {
public:
    ServoController ctr;
protected:
    void SetUp() override {
        EXPECT_EQ(ctr.begin(), 1);
        ctr.start();
        EXPECT_TRUE(ctr.isRunning());
    }
    void TearDown() override {
        EXPECT_EQ(ctr.end(), 1);
    }
};

// Tests ServoController default values already exist
TEST_F(ServoControllerTest, ServoDefaultValuesExist) {
    EXPECT_TRUE(ctr.isOn(SERVO_HORIZONTAL));
    EXPECT_TRUE(ctr.isOn(SERVO_VERTICAL));
    EXPECT_EQ((int)ctr.getValue(SERVO_HORIZONTAL), SERVO_DEFAULT_ANGLE);
    EXPECT_EQ((int)ctr.getValue(SERVO_VERTICAL), SERVO_DEFAULT_ANGLE);
}

// Tests ServoController adding action
TEST_F(ServoControllerTest, AddActionWorks) {
    ServoAction a1 {SERVO_HORIZONTAL, -10};
    ServoAction a2 {SERVO_VERTICAL, 10};

    ctr.addActions({a1, a2});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ((int)ctr.getValue(a1.moveId), SERVO_DEFAULT_ANGLE + a1.angle);
    EXPECT_EQ((int)ctr.getValue(a2.moveId), SERVO_DEFAULT_ANGLE + a2.angle);

    a1.angle = -90;
    a2.angle = 90;
    ctr.addActions({a1, a2});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(ctr.getValue(a1.moveId), 0u);
    EXPECT_EQ((int)ctr.getValue(a2.moveId), SERVO_MAX_ANGLE);
}

// Tests ServoController rotate method works
TEST_F(ServoControllerTest, RotateWorks) {
    ctr.rotate(DroneCamera::CAMERA_X_AXE, 50);
    ctr.rotate(DroneCamera::CAMERA_Y_AXE, -50);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ((int)ctr.getValue(SERVO_HORIZONTAL), SERVO_DEFAULT_ANGLE + 50);
    EXPECT_EQ((int)ctr.getValue(SERVO_VERTICAL), SERVO_DEFAULT_ANGLE - 50);
}