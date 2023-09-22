#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <ClientSender.h>
#include <ClientReceiver.h>
#include <BatteryReceiver.h>
#include <stream/StreamReceiver.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;
using namespace stream;

class ClientReceiverTest : public ::testing::Test {
public:
    ClientSender sender;
    BatteryReceiver batRecv;
    ClientReceiver pcrcv;
    StreamReceiver astream;
    StreamReceiver vstream;
protected:
    ClientReceiverTest() : batRecv(), pcrcv(batRecv, sender, vstream, astream) {}
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();

        Config::setInt(DRONE_PORT_SEND, VAR_DRONE_PORT);
        Config::setString(DRONE_ADDRESS, VAR_DRONE_ADDRESS);
        Config::setInt(CLIENT_PORT_RCV, VAR_RCV_PORT);
        Config::setInt(NET_FRAGMENT_SIZE, MAX_FRAGMENT_SIZE);
        Config::setInt(NET_FRAGMENT_NUMBER, MAX_FRAGMENT_NUMBER);
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

// Tests ClientReceiver begin with default config
TEST_F(ClientReceiverTest, BeginWithDefaultConfigWorks) {
    EXPECT_EQ(pcrcv.begin(), 1);
    EXPECT_EQ(pcrcv.end(), 1);
}

// Tests ClientReceiver end
TEST_F(ClientReceiverTest, EndServiceWorks) {
    EXPECT_EQ(pcrcv.begin(), 1);
    pcrcv.start();
    EXPECT_EQ(pcrcv.end(), 1);
}