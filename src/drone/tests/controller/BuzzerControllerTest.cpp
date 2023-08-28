#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include <component/Buzzer.h>
#include <controller/BuzzerController.h>
#include <utils/Structs.h>

#include <rpi_ws281x/ws2811.h>

using namespace controller;
using namespace component;
using namespace utils;

class BuzzerControllerTest : public ::testing::Test {
public:
    BuzzerController ctr;
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

// Tests BuzzerController turning on/off is working
TEST_F(BuzzerControllerTest, TurnOnOffWorks) {
    BuzzerAction a;
    a.on = true;

    ctr.addAction(a);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(BUZZER_ID));

    a.on = false;
    ctr.addAction(a);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));
}

// Tests BuzzerController turning on/off with delay is working
TEST_F(BuzzerControllerTest, TurnOnOffWithDelayWorks) {
    BuzzerAction a;
    a.start = 400;
    a.delay = 500;
    a.on = true;

    ctr.addAction(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));

    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    EXPECT_TRUE(ctr.isOn(BUZZER_ID));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));
}

// Tests BuzzerController turning on/off with delay and repeat is working
TEST_F(BuzzerControllerTest, TurnOnOffWithDelayRepeatWorks) {
    BuzzerAction a;
    a.start = 0;
    a.delay = 500;
    a.repeat = 1;
    a.on = true;

    ctr.addAction(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(BUZZER_ID));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(ctr.isOn(BUZZER_ID));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));
}

// Tests BuzzerController turn on/off Buzzer according to the app state
TEST_F(BuzzerControllerTest, PlayStateWorks) {
    ctr.play(APP_DISCOVERING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(BUZZER_ID));

    ctr.play(APP_RUNNING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));

    ctr.play(APP_ERROR);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(BUZZER_ID));
}

// Tests BuzzerController turn on works
TEST_F(BuzzerControllerTest, TurnWorks) {
    ctr.turn(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(BUZZER_ID));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(BUZZER_ID));
}