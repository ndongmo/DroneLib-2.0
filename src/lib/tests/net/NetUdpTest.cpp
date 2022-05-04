#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <net/NetUdp.h>

using namespace net;

class NetUdpTest : public ::testing::Test {
protected:
    const char* FAKE_ADDRESS = "192.168..1";
    const char* CLIENT_ADDRESS = "127.0.0.1";
    const char* SERVER_ADDRESS = "127.0.0.1";
    const int CLIENT_PORT = 1239;
    const int SERVER_PORT = 1240;
};

// Tests Udp open method is working
TEST_F(NetUdpTest, OpenWorks) {
    NetUdp udp;
    EXPECT_EQ(udp.open(CLIENT_ADDRESS, CLIENT_PORT, SERVER_ADDRESS, SERVER_PORT), 1);
}

// Tests Udp open method with empty server address is working
TEST_F(NetUdpTest, OpenWorksWithEmptyServerAddr) {
    NetUdp udp;
    EXPECT_EQ(udp.open(CLIENT_ADDRESS, CLIENT_PORT, "", SERVER_PORT), 1);
}

// Tests Udp open method is not working with fake address
TEST_F(NetUdpTest, OpenFakeAddressNotWorks) {
    NetUdp udp;
    EXPECT_EQ(udp.open(FAKE_ADDRESS, CLIENT_PORT, SERVER_ADDRESS, SERVER_PORT), -1);
    EXPECT_EQ(udp.open(CLIENT_ADDRESS, CLIENT_PORT, FAKE_ADDRESS, SERVER_PORT), -1);
}

// Tests Udp send and receive method work
TEST_F(NetUdpTest, SendReceiveWork) {
    int rcv_len = 10, send_len;
    NetUdp udp1, udp2;
    const char* sendBuffer = "test";
    char recvBuffer[rcv_len];

    send_len = sizeof(sendBuffer);

    EXPECT_EQ(udp1.open(CLIENT_ADDRESS, CLIENT_PORT, SERVER_ADDRESS, SERVER_PORT), 1);
    EXPECT_EQ(udp2.open(SERVER_ADDRESS, SERVER_PORT, CLIENT_ADDRESS, CLIENT_PORT), 1);

    EXPECT_EQ(udp1.send(sendBuffer, send_len), send_len);
    EXPECT_EQ(udp2.receive(recvBuffer, rcv_len), send_len);

    EXPECT_EQ(udp2.send(sendBuffer, send_len), send_len);
    EXPECT_EQ(udp1.receive(recvBuffer, rcv_len), send_len);
}