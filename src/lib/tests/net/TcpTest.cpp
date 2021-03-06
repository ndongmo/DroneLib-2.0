#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>

#include <net/Udp.h>
#include <net/Tcp.h>
#include <net/NetworkHelper.h>

using namespace net;

void runOpenClient(Tcp *tcp, const char *serverAddr, int serverPort) {
    EXPECT_EQ(tcp->openClient(serverAddr, serverPort), 1);
}
void runReceive(Tcp *tcp, char* recvBuffer, int rcv_len, int send_len) {
    EXPECT_EQ(tcp->receive(recvBuffer, rcv_len), send_len);
}

class TcpTest : public ::testing::Test {
protected:
    const char* FAKE_ADDRESS = "192.168..1";
    const char* CLIENT_ADDRESS = "127.0.0.1";
    const char* SERVER_ADDRESS = "127.0.0.1";
    const int CLIENT_PORT = 1239;
    const int SERVER_PORT = 1240;
};

// Tests Tcp open method is working
TEST_F(TcpTest, OpenWorks) {
    Tcp tcp1, tcp2;
    struct sockaddr_in client;
    
    EXPECT_EQ(tcp1.openServer(SERVER_ADDRESS, SERVER_PORT), 1);
    //call openClient in a different thread so that the listen should start
    //before the opening (IMPORTANT)
    std::thread openProcess(runOpenClient, &tcp2, SERVER_ADDRESS, SERVER_PORT);
    EXPECT_EQ(tcp1.listen(client), 1);
    openProcess.join();

    ASSERT_EQ(NetworkHelper::getIpv4Addr(client), SERVER_ADDRESS);
}

// Tests Tcp open method with empty server address is working
TEST_F(TcpTest, OpenWorksWithEmptyServerAddrWorks) {
    Tcp tcp;
    EXPECT_EQ(tcp.openServer("", SERVER_PORT), 1);
}

// Tests Tcp open method is not working with fake address
TEST_F(TcpTest, OpenFakeAddressNotWorks) {
    Tcp tcp;
    EXPECT_EQ(tcp.openServer(FAKE_ADDRESS, SERVER_PORT), -1);
}

// Tests Tcp send and receive method work
TEST_F(TcpTest, SendReceiveWork) {
    int rcv_len = 10, send_len;
    Tcp tcp1, tcp2;
    struct sockaddr_in client;
    const char* sendBuffer = "test";
    char recvBuffer[rcv_len];

    send_len = sizeof(sendBuffer);

    EXPECT_EQ(tcp1.openServer(SERVER_ADDRESS, SERVER_PORT), 1);
    //call openClient in a different thread so that the listen should start
    //before the opening (IMPORTANT)
    std::thread openProcess(runOpenClient, &tcp2, SERVER_ADDRESS, SERVER_PORT);
    EXPECT_EQ(tcp1.listen(client), 1);
    openProcess.join();

    std::thread receiveProcess1(runReceive, &tcp2, &recvBuffer[0], rcv_len, send_len);
    EXPECT_EQ(tcp1.send(sendBuffer, send_len), send_len);
    receiveProcess1.join();

    std::thread receiveProcess2(runReceive, &tcp1, &recvBuffer[0], rcv_len, send_len);
    EXPECT_EQ(tcp2.send(sendBuffer, send_len), send_len);
    receiveProcess2.join();
}