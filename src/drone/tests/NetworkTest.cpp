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

void runDroneDiscovery(Network* droneNet) {
    EXPECT_EQ(droneNet->discovery(), 1);
}

void runDroneBegin(Network* droneNet) {
    EXPECT_EQ(droneNet->begin(), 1);
}

void runClientDiscovery(const char* droneAddr, int dronePort, int clientRcvPort,
    int droneRcvPortExp, int maxFragmentSizeExp, int maxFragmentNumberExp) {
    
    Tcp tcpClient;
    EXPECT_EQ(tcpClient.openClient(droneAddr, dronePort), 1);

    nlohmann::json json = {
        {CTRL_PORT_RCV, clientRcvPort}
    };
    std::string msg = json.dump();

    EXPECT_GT(tcpClient.send(msg.c_str(), msg.length()), 0);

    char buf[1024];

    EXPECT_GT(tcpClient.receive(buf, 1024), 0);

    int droneRcvPort, maxFragmentSize, maxFragmentNumber;

	try {
        json = nlohmann::json::parse(buf);
		json[DRONE_PORT_RCV].get_to(droneRcvPort);
		json[NET_FRAGMENT_SIZE].get_to(maxFragmentSize);
        json[NET_FRAGMENT_NUMBER].get_to(maxFragmentNumber);
	}
	catch (...) {
		FAIL();
	}

    ASSERT_EQ(droneRcvPort, droneRcvPortExp);
    ASSERT_EQ(maxFragmentSize, maxFragmentSizeExp);
    ASSERT_EQ(maxFragmentNumber, maxFragmentNumberExp);
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
    int clientRcvPort = 1555;
    Network droneNet;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    
    std::thread droneProcess(runDroneDiscovery, &droneNet);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess(runClientDiscovery, droneAddr.c_str(), DRONE_PORT_DISCOVERY_DEFAULT, 
        clientRcvPort, DRONE_PORT_RCV_DEFAULT, NET_FRAGMENT_SIZE_DEFAULT, NET_FRAGMENT_NUMBER_DEFAULT);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(droneAddr, droneNet.getClientAddr());
    ASSERT_EQ(clientRcvPort, droneNet.getClientRcvPort());
}

// Tests Network begin with default config
TEST_F(NetworkTest, BeginWithDefaultConfigWorks) {
    std::string droneAddr = "127.0.0.1";
    int clientRcvPort = 1555;
    Network droneNet;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    
    std::thread droneProcess(runDroneBegin, &droneNet);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess(runClientDiscovery, droneAddr.c_str(), DRONE_PORT_DISCOVERY_DEFAULT, 
        clientRcvPort, DRONE_PORT_RCV_DEFAULT, NET_FRAGMENT_SIZE_DEFAULT, NET_FRAGMENT_NUMBER_DEFAULT);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(droneAddr, droneNet.getClientAddr());
    ASSERT_EQ(clientRcvPort, droneNet.getClientRcvPort());
}