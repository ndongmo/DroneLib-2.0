#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include <component/Led.h>
#include <controller/LedController.h>
#include <utils/Structs.h>

#include <rpi_ws281x/ws2811.h>

using namespace controller;
using namespace component;
using namespace utils;

// Mock class of the LedController
class MockLedController : public LedController {
public:
    unsigned int getStateLeds(int i) {
        return m_state_leds[i];
    }
};

class LedControllerTest : public ::testing::Test {
public:
    MockLedController ctr;
protected:
    void SetUp() override {
        EXPECT_EQ(ctr.begin(), 1);
        ctr.start();
        EXPECT_TRUE(ctr.isRunning());
    }
    void TearDown() override {
        EXPECT_EQ(ctr.end(), 1);
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_RCV_PORT = 1555;
    const int VAR_DRONE_SEND_PORT = 1556;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests LedController turning on/off is working
TEST_F(LedControllerTest, TurnOnOffWorks) {
    LedAction a;
    a.ledId = 0;
    a.color = LedColorIndex::ID_BLUE;
    a.on = true;

    ctr.addAction(a);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(a.ledId));

    a.on = false;
    ctr.addAction(a);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(ctr.isOn(a.ledId));
}

// Tests LedController turning on/off with delay is working
TEST_F(LedControllerTest, TurnOnOffWithDelayWorks) {
    LedAction a;
    a.ledId = 0;
    a.color = LedColorIndex::ID_BLUE;
    a.start = 400;
    a.delay = 500;
    a.on = true;

    ctr.addAction(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(ctr.isOn(a.ledId));

    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    EXPECT_TRUE(ctr.isOn(a.ledId));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(a.ledId));
}

// Tests LedController turning on/off with delay and repeat is working
TEST_F(LedControllerTest, TurnOnOffWithDelayRepeatWorks) {
    LedAction a1, a2;
    a1.ledId = 0;
    a2.ledId = 1;
    a1.color = LedColorIndex::ID_BLUE;
    a2.color = LedColorIndex::ID_GREEN;
    a1.start = 0;
    a2.start = 500;
    a1.delay = 500;
    a2.delay = 500;
    a1.repeat = 1;
    a2.repeat = 1;
    a1.on = true;
    a2.on = true;
    std::vector<LedAction> list {a1, a2};

    ctr.addActions(list);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ctr.isOn(a1.ledId));
    EXPECT_FALSE(ctr.isOn(a2.ledId));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(a1.ledId));
    EXPECT_TRUE(ctr.isOn(a2.ledId));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(ctr.isOn(a1.ledId));
    EXPECT_FALSE(ctr.isOn(a2.ledId));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(a1.ledId));
    EXPECT_TRUE(ctr.isOn(a2.ledId));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(ctr.isOn(a1.ledId));
    EXPECT_FALSE(ctr.isOn(a2.ledId));
}

// Tests LedController turn on/off led according to the app state
TEST_F(LedControllerTest, PlayStateWorks) {
    ctr.play(APP_DISCOVERING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(ctr.getStateLeds(0)) || ctr.isOn(ctr.getStateLeds(1)));

    ctr.play(APP_RUNNING);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(ctr.getStateLeds(0)) && ctr.isOn(ctr.getStateLeds(1)));

    ctr.play(APP_ERROR);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ctr.isOn(ctr.getStateLeds(0)) || ctr.isOn(ctr.getStateLeds(1)));
}