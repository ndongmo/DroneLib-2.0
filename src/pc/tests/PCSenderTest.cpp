#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <PCSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;

class PCSenderTest : public ::testing::Test {
public:
    PCSender pcsender;
protected:
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();

        Config::setIntVar(DRONE_PORT_RCV, VAR_DRONE_PORT);
        Config::setIntVar(CTRL_PORT_SEND, VAR_SEND_PORT);
        Config::setStringVar(DRONE_ADDRESS, VAR_DRONE_ADDRESS);
	    Config::setIntVar(NET_FRAGMENT_SIZE, MAX_FRAGMENT_SIZE);
	    Config::setIntVar(NET_FRAGMENT_NUMBER, MAX_FRAGMENT_NUMBER);
    }
    void TearDown() override {
        remove(CONFIG_FILE);
        pcsender.end();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_PORT = 1535;
    const int VAR_SEND_PORT = 1536;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests PCSender begin with default config
TEST_F(PCSenderTest, BeginWithDefaultConfigWorks) {
    EXPECT_EQ(pcsender.begin(), 1);
    EXPECT_EQ(pcsender.end(), 1);
}

// Tests PCSender end
TEST_F(PCSenderTest, EndServiceWorks) {
    EXPECT_EQ(pcsender.begin(), 1);
    pcsender.start();
    EXPECT_EQ(pcsender.end(), 1);
}