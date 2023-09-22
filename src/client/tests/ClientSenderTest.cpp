#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <ClientSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;

class ClientSenderTest : public ::testing::Test {
public:
    ClientSender client;
protected:
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();

        Config::setInt(DRONE_PORT_RCV, VAR_DRONE_PORT);
        Config::setInt(CLIENT_PORT_SEND, VAR_SEND_PORT);
        Config::setString(DRONE_ADDRESS, VAR_DRONE_ADDRESS);
	    Config::setInt(NET_FRAGMENT_SIZE, MAX_FRAGMENT_SIZE);
	    Config::setInt(NET_FRAGMENT_NUMBER, MAX_FRAGMENT_NUMBER);
    }
    void TearDown() override {
        remove(CONFIG_FILE);
        client.end();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_PORT = 1535;
    const int VAR_SEND_PORT = 1536;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests client begin with default config
TEST_F(ClientSenderTest, BeginWithDefaultConfigWorks) {
    EXPECT_EQ(client.begin(), 1);
    EXPECT_EQ(client.end(), 1);
}

// Tests ClientSender end
TEST_F(ClientSenderTest, EndServiceWorks) {
    EXPECT_EQ(client.begin(), 1);
    client.start();
    EXPECT_EQ(client.end(), 1);
}