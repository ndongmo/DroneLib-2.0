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
public:
    DroneController ctrl;
protected:
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();
    }
    void TearDown() override {
        remove(CONFIG_FILE);
        ctrl.end();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_CLIENT_RCV_PORT = 1555;
    const int VAR_DRONE_SEND_PORT = 1556;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
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
    std::thread droneProcess(runCtrlDiscovery, &ctrl);

    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::thread clientProcess(runClientDiscovery, VAR_DRONE_ADDRESS, 
        DRONE_PORT_DISCOVERY_DEFAULT, VAR_CLIENT_RCV_PORT, DRONE_PORT_RCV_DEFAULT, 
        DRONE_PORT_SEND_DEFAULT, NET_FRAGMENT_SIZE_DEFAULT, NET_FRAGMENT_NUMBER_DEFAULT);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(VAR_DRONE_ADDRESS, ctrl.getClientAddr());
    ASSERT_EQ(VAR_CLIENT_RCV_PORT, ctrl.getClientRcvPort());
    ASSERT_EQ(1, ctrl.end());
}

// Tests Network begin with default config
TEST_F(DroneControllerTest, BeginWithDefaultConfigWorks) {
    std::thread droneProcess(runCtrlBegin, &ctrl);

    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::thread clientProcess(runClientDiscovery, VAR_DRONE_ADDRESS, 
        DRONE_PORT_DISCOVERY_DEFAULT, VAR_CLIENT_RCV_PORT, DRONE_PORT_RCV_DEFAULT, 
        DRONE_PORT_SEND_DEFAULT, NET_FRAGMENT_SIZE_DEFAULT, NET_FRAGMENT_NUMBER_DEFAULT);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(VAR_DRONE_ADDRESS, ctrl.getClientAddr());
    ASSERT_EQ(VAR_CLIENT_RCV_PORT, ctrl.getClientRcvPort());
    ASSERT_EQ(1, ctrl.end());
}