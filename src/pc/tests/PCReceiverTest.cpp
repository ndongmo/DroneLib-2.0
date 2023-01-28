#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <PCSender.h>
#include <PCReceiver.h>
#include <stream/StreamReceiver.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;
using namespace stream;

class PCReceiverTest : public ::testing::Test {
public:
    PCSender sender;
    PCReceiver pcrcv;
    StreamReceiver vstream;
protected:
    PCReceiverTest() : pcrcv(sender, vstream) {}
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();

        Config::setIntVar(DRONE_PORT_SEND, VAR_DRONE_PORT);
        Config::setStringVar(DRONE_ADDRESS, VAR_DRONE_ADDRESS);
        Config::setIntVar(CTRL_PORT_RCV, VAR_RCV_PORT);
        Config::setIntVar(NET_FRAGMENT_SIZE, MAX_FRAGMENT_SIZE);
        Config::setIntVar(NET_FRAGMENT_NUMBER, MAX_FRAGMENT_NUMBER);
    }
    void TearDown() override {
        remove(CONFIG_FILE);
        pcrcv.end();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_PORT = 1525;
    const int VAR_RCV_PORT = 1526;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests PCReceiver begin with default config
TEST_F(PCReceiverTest, BeginWithDefaultConfigWorks) {
    EXPECT_EQ(pcrcv.begin(), 1);
    EXPECT_EQ(pcrcv.end(), 1);
}

// Tests PCReceiver end
TEST_F(PCReceiverTest, EndServiceWorks) {
    EXPECT_EQ(pcrcv.begin(), 1);
    pcrcv.start();
    EXPECT_EQ(pcrcv.end(), 1);
}