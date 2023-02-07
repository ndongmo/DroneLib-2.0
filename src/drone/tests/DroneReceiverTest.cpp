#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <DroneSender.h>
#include <DroneReceiver.h>
#include <controller/MotorController.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;
using namespace controller;

class DroneReceiverTest : public ::testing::Test {
public:
    DroneSender sender;
    DroneReceiver droneRcv;
    MotorController motorCtrl;
    ServoController servoCtrl;
protected:
    DroneReceiverTest() : droneRcv(sender, motorCtrl, servoCtrl) {}
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();

        Config::setString(DRONE_ADDRESS, VAR_DRONE_ADDRESS);
        Config::setString(CTRL_ADDRESS, VAR_DRONE_ADDRESS);
        Config::setInt(DRONE_PORT_RCV, VAR_DRONE_PORT);
        Config::setInt(CTRL_PORT_RCV, VAR_RCV_PORT);
        Config::setInt(NET_FRAGMENT_SIZE, MAX_FRAGMENT_SIZE);
        Config::setInt(NET_FRAGMENT_NUMBER, MAX_FRAGMENT_NUMBER);
    }
    void TearDown() override {
        remove(CONFIG_FILE);
        droneRcv.end();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_PORT = 1255;
    const int VAR_RCV_PORT = 1526;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests Network begin with default config
TEST_F(DroneReceiverTest, BeginWithDefaultConfigWorks) {
    EXPECT_EQ(droneRcv.begin(), 1);
    EXPECT_EQ(droneRcv.end(), 1);
}