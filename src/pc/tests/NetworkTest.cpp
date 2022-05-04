#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>
#include <chrono>

#include <Network.h>
#include <Constants.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <nlohmann/json.hpp>

using namespace utils;

void runPCDiscovery(Network* pcNet) {
    EXPECT_EQ(pcNet->discovery(), 1);
}

void runPCBegin(Network* pcNet) {
    EXPECT_EQ(pcNet->begin(), 1);
}

void runDroneDiscovery(const char* discoveryAddr, int discoveryPort, int clientRcvPortExp,
    const char* clientAddr, int droneRcvPort, int maxFragmentSize, int maxFragmentNumber) {
    
    Tcp tcpDrone;
    int clientRcvPort;
    struct sockaddr_in client;

    EXPECT_EQ(tcpDrone.openServer(discoveryAddr, discoveryPort), 1);
    EXPECT_EQ(tcpDrone.listen(client), 1);

    ASSERT_EQ(NetworkHelper::getIpv4Addr(client), clientAddr);

    char buf[1024];
	nlohmann::json json;

    EXPECT_GT(tcpDrone.receive(buf, 1024), 0);

    try {
        json = nlohmann::json::parse(buf);
		json[CTRL_PORT_RCV].get_to(clientRcvPort);
	}
	catch (...) {
		FAIL();
	}

    ASSERT_EQ(clientRcvPort, clientRcvPortExp);

    json = {
        {DRONE_PORT_RCV, Config::getInt(DRONE_PORT_RCV, droneRcvPort)},
		{NET_FRAGMENT_SIZE, Config::getInt(NET_FRAGMENT_SIZE, maxFragmentSize)},
		{NET_FRAGMENT_NUMBER, Config::getInt(NET_FRAGMENT_NUMBER, maxFragmentNumber)}
    };
    std::string msg = json.dump();

    EXPECT_GT(tcpDrone.send(msg.c_str(), msg.length()), 0);
}

class NetworkTest : public ::testing::Test {
protected:
    void TearDown() override {
        remove(CONFIG_FILE);
    }
};

// Tests Network discovery with default config
TEST_F(NetworkTest, DiscoveryWithDefaultConfigWorks) {
    std::string droneAddr = "127.0.0.1";
    int droneRcvPort = 1555, maxFragmentSize = 800, maxFragmentNumber = 256;
    Network pcNet;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread droneProcess(runDroneDiscovery, droneAddr.c_str(), DRONE_PORT_DISCOVERY_DEFAULT, 
        CTRL_PORT_RCV_DEFAULT, droneAddr.c_str(), droneRcvPort, maxFragmentSize, maxFragmentNumber);

     // sleep 100 milliseconds
    std::thread clientProcess(runPCDiscovery, &pcNet);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(droneRcvPort, pcNet.getDroneRcvPort());
    ASSERT_EQ(maxFragmentSize, pcNet.getMaxFragementSize());
    ASSERT_EQ(maxFragmentNumber, pcNet.getMaxFragementNumber());
}

// Tests Network begin with default config
TEST_F(NetworkTest, BeginWithDefaultConfigWorks) {
    std::string droneAddr = "127.0.0.1";
    int droneRcvPort = 1555, maxFragmentSize = 800, maxFragmentNumber = 256;
    Network pcNet;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread droneProcess(runDroneDiscovery, droneAddr.c_str(), DRONE_PORT_DISCOVERY_DEFAULT, 
        CTRL_PORT_RCV_DEFAULT, droneAddr.c_str(), droneRcvPort, maxFragmentSize, maxFragmentNumber);

    // sleep 100 milliseconds
    std::thread clientProcess(runPCBegin, &pcNet);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(droneRcvPort, pcNet.getDroneRcvPort());
    ASSERT_EQ(maxFragmentSize, pcNet.getMaxFragementSize());
    ASSERT_EQ(maxFragmentNumber, pcNet.getMaxFragementNumber());
}