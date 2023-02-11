#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include <utils/Constants.h>
#include <net/NetSender.h>
#include <controller/BatteryController.h>

using namespace controller;

class MockBatteryController : public BatteryController {
public:
    MockBatteryController(NetSender &sender) : BatteryController(sender) {}
    int getDelay() { return m_delay; }
private:
    void sendLifePercentage() override { }
};

class BatteryControllerTest : public ::testing::Test {
public:
    NetSender sender;
    MockBatteryController ctr;
protected:
    BatteryControllerTest() : ctr(sender) {}
    void SetUp() override {
        EXPECT_EQ(ctr.begin(), 1);
        ctr.start();
        EXPECT_TRUE(ctr.isRunning());
    }
    void TearDown() override {
        EXPECT_EQ(ctr.end(), 1);
    }
};

// Tests BatteryController running process
TEST_F(BatteryControllerTest, RunWorks) {
    EXPECT_EQ(BATTERY_LAPS, ctr.getDelay());
    std::this_thread::sleep_for(std::chrono::milliseconds(BATTERY_LAPS + 100));
    EXPECT_TRUE(ctr.isOn(0));
}