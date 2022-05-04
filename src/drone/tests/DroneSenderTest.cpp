#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <DroneSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;

class DroneSenderTest : public ::testing::Test {
protected:
    void TearDown() override {
        remove(CONFIG_FILE);
    }
};

// Tests DroneSender begin with default config
TEST_F(DroneSenderTest, BeginWithDefaultConfigWorks) {
    std::string droneAddr = "127.0.0.1";
    int clientRcvPort = 1555, maxFragmentSize = 800, maxFragmentNumber = 256;
    DroneSender droneSender;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    droneSender.init(clientRcvPort, droneAddr, maxFragmentSize, maxFragmentNumber);

    EXPECT_EQ(droneSender.begin(), 1);
}