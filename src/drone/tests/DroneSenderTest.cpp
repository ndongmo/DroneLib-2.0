#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <DroneSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;

class DroneSenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();
    }
    void TearDown() override {
        remove(CONFIG_FILE);
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_PORT = 1555;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests DroneSender begin with default config
TEST_F(DroneSenderTest, BeginWithDefaultConfigWorks) {
    DroneSender droneSender;
    droneSender.init(VAR_DRONE_PORT, VAR_DRONE_ADDRESS, MAX_FRAGMENT_SIZE, MAX_FRAGMENT_NUMBER);

    EXPECT_EQ(droneSender.begin(), 1);
    EXPECT_EQ(droneSender.end(), 1);
}