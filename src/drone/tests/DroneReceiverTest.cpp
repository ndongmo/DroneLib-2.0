#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <DroneReceiver.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;

class DroneReceiverTest : public ::testing::Test {
protected:
    void TearDown() override {
        remove(CONFIG_FILE);
    }
};

// Tests Network begin with default config
TEST_F(DroneReceiverTest, BeginWithDefaultConfigWorks) {
    std::string droneAddr = "127.0.0.1";
    int clientRcvPort = 1555, maxFragmentSize = 800, maxFragmentNumber = 256;
    DroneReceiver droneRcv;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    droneRcv.init(clientRcvPort, droneAddr, maxFragmentSize, maxFragmentNumber);

    EXPECT_EQ(droneRcv.begin(), 1);
}