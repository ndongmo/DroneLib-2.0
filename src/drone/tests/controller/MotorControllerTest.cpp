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
    void TearDown() override {
        ctr.end();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_RCV_PORT = 1555;
    const int VAR_DRONE_SEND_PORT = 1556;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests MotorController start method is working
TEST_F(MotorControllerTest, StartWorks) {
    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());
    EXPECT_EQ(ctr.end(), 1);
}

// Tests MotorController moving and stopping is working
TEST_F(MotorControllerTest, MoveAndStopMotorWorks) {
    MotorAction a {WHEEL_TL_FORWARD, SPEED_MIN, DRONE_WHEEL_MOVE_LAPS};

    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());

    ctr.addAction(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_TRUE(ctr.isOn(a.moveId));

    a.speed = 0;
    ctr.addAction(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_FALSE(ctr.isOn(a.moveId));

    EXPECT_EQ(ctr.end(), 1);
}

// Tests MotorController moving forward is working
TEST_F(MotorControllerTest, MoveForwardWorks) {
    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());

    ctr.move(DIR_FORWARD, SPEED_MIN);
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

    EXPECT_EQ(ctr.end(), 1);
}

// Tests MotorController moving backward is overriden by moving forward
TEST_F(MotorControllerTest, BackwardOverridenByForwardWorks) {
    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());

    ctr.move(DIR_FORWARD, SPEED_MIN);
    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS / 3));
    EXPECT_TRUE(ctr.isOn(WHEEL_TR_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_TL_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BR_FORWARD));
    EXPECT_TRUE(ctr.isOn(WHEEL_BL_FORWARD));

    ctr.move(DIR_BACKWARD, SPEED_MIN);

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

    EXPECT_EQ(ctr.end(), 1);
}