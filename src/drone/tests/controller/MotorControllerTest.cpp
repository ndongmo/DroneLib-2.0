#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include <utils/Constants.h>

#include <component/Motor.h>
#include <controller/MotorController.h>

using namespace controller;
using namespace component;

class MotorControllerTest : public ::testing::Test {
public:
    MotorController ctr;
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

// Tests MotorController moving and stopping is working
TEST_F(MotorControllerTest, MoveAndStopMotorWorks) {
    MotorAction a {WHEEL_TL_FORWARD, SPEED_LOW, DRONE_WHEEL_MOVE_LAPS};

    ctr.addAction(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_TRUE(ctr.isOn(a.moveId));

    a.speed = 0;
    ctr.addAction(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_FALSE(ctr.isOn(a.moveId));
}

// Tests MotorController moving forward is working
TEST_F(MotorControllerTest, MoveForwardWorks) {
    ctr.move(DIR_FORWARD, SPEED_LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_TRUE(ctr.isOn(WHEEL_TR_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_TL_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BR_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BL_FORWARD));

    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS));
    EXPECT_FALSE(ctr.isOn(WHEEL_TR_FORWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_TL_FORWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_BR_FORWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_BL_FORWARD));
}

// Tests MotorController moving backward is overriden by moving forward
TEST_F(MotorControllerTest, BackwardOverridenByForwardWorks) {
    ctr.move(DIR_FORWARD, SPEED_LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_TRUE(ctr.isOn(WHEEL_TR_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_TL_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BR_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BL_FORWARD));

    ctr.move(DIR_BACKWARD, SPEED_LOW);

    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_FALSE(ctr.isOn(WHEEL_TR_FORWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_TL_FORWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_BR_FORWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_BL_FORWARD));

    EXPECT_TRUE(ctr.isOn(WHEEL_TR_BACKWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_TL_BACKWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BR_BACKWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BL_BACKWARD));

    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS));
    EXPECT_FALSE(ctr.isOn(WHEEL_TR_BACKWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_TL_BACKWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_BR_BACKWARD));
    EXPECT_FALSE(ctr.isOn(WHEEL_BL_BACKWARD));
}