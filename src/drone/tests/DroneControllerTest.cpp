#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>
#include <chrono>

#include <DroneController.h>
#include <Constants.h>
#include <net/NetTcp.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <nlohmann/json.hpp>

using namespace utils;
using namespace net;

class DroneControllerTest : public ::testing::Test {
protected:
    void TearDown() override {
        remove(CONFIG_FILE);
    }
};

void runCtrlDiscovery(DroneController* ctrl) {
    EXPECT_EQ(ctrl->discovery(), 1);
}

void runCtrlBegin(DroneController* ctrl) {
    EXPECT_EQ(ctrl->begin(), 1);
}

void runClientDiscovery(const char* droneAddr, int dronePort, int clientRcvPort,
    int droneRcvPortExp, int droneSendPortExp, int maxFragmentSizeExp, int maxFragmentNumberExp) {
    
    NetTcp tcpClient;
    EXPECT_EQ(tcpClient.openClient(droneAddr, dronePort), 1);

    nlohmann::json json = {
        {CTRL_PORT_RCV, clientRcvPort}
    };
    std::string msg = json.dump();

    EXPECT_GT(tcpClient.send(msg.c_str(), msg.length()), 0);

    char buf[1024];
    int droneRcvPort, droneSendPort, maxFragmentSize, maxFragmentNumber;

    EXPECT_GT(tcpClient.receive(buf, 1024), 0);

	try {
        json = nlohmann::json::parse(buf);
        json[DRONE_PORT_RCV].get_to(droneRcvPort);
        json[DRONE_PORT_SEND].get_to(droneSendPort);
		json[NET_FRAGMENT_SIZE].get_to(maxFragmentSize);
        json[NET_FRAGMENT_NUMBER].get_to(maxFragmentNumber);
	}
	catch (...) {
		FAIL();
	}

    ASSERT_EQ(droneRcvPort, droneRcvPortExp);
    ASSERT_EQ(droneSendPort, droneSendPortExp);
    ASSERT_EQ(maxFragmentSize, maxFragmentSizeExp);
    ASSERT_EQ(maxFragmentNumber, maxFragmentNumberExp);
}

// Tests Network discovery with default config
TEST_F(DroneControllerTest, DiscoveryWithDefaultConfigWorks) {
    std::string droneAddr = "127.0.0.1";
    int clientRcvPort = 1555;
    DroneController ctrl;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    
    std::thread droneProcess(runCtrlDiscovery, &ctrl);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess(runClientDiscovery, droneAddr.c_str(), 
        DRONE_PORT_DISCOVERY_DEFAULT, clientRcvPort, DRONE_PORT_RCV_DEFAULT, 
        DRONE_PORT_SEND_DEFAULT, NET_FRAGMENT_SIZE_DEFAULT, NET_FRAGMENT_NUMBER_DEFAULT);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(droneAddr, ctrl.getClientAddr());
    ASSERT_EQ(clientRcvPort, ctrl.getClientRcvPort());
}

// Tests Network begin with default config
TEST_F(DroneControllerTest, BeginWithDefaultConfigWorks) {
    std::string droneAddr = "127.0.0.1";
    int clientRcvPort = 1555;
    DroneController ctrl;

    std::ofstream configFile(CONFIG_FILE);
    configFile << "{\"" << DRONE_ADDRESS << "\":\"" << droneAddr << "\"}";
    configFile.close();
    Config::init();
    
    std::thread droneProcess(runCtrlBegin, &ctrl);
    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess(runClientDiscovery, droneAddr.c_str(), 
        DRONE_PORT_DISCOVERY_DEFAULT, clientRcvPort, DRONE_PORT_RCV_DEFAULT, 
        DRONE_PORT_SEND_DEFAULT, NET_FRAGMENT_SIZE_DEFAULT, NET_FRAGMENT_NUMBER_DEFAULT);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(droneAddr, ctrl.getClientAddr());
    ASSERT_EQ(clientRcvPort, ctrl.getClientRcvPort());
}