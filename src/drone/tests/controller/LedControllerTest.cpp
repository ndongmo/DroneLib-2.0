#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include <component/Led.h>
#include <controller/LedController.h>

#include <rpi_ws281x/ws2811.h>

using namespace controller;
using namespace component;

class LedControllerTest : public ::testing::Test {
public:
    LedController ctr;
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

// Tests LedController start method is working
TEST_F(LedControllerTest, StartWorks) {
    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());
    EXPECT_EQ(ctr.end(), 1);
}

// Tests LedController turning on/off is working
TEST_F(LedControllerTest, TurnOnOffWorks) {
    LedAction a;

    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());

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

    EXPECT_EQ(ctr.end(), 1);
}

// Tests LedController turning on/off with delay is working
TEST_F(LedControllerTest, TurnOnOffWithDelayWorks) {
    LedAction a;

    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());

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

    EXPECT_EQ(ctr.end(), 1);
}

// Tests LedController turning on/off with delay and repeat is working
TEST_F(LedControllerTest, TurnOnOffWithDelayRepeatWorks) {
    LedAction a1, a2;

    EXPECT_EQ(ctr.begin(), 1);
    ctr.start();
    EXPECT_TRUE(ctr.isRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

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

    EXPECT_EQ(ctr.end(), 1);
}