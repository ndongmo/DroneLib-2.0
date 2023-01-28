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
#include <controller/LedController.h>

using namespace utils;
using namespace net;
using namespace controller;

// Mock class of the DroneController
class MockDroneController : public DroneController {
public:
    LedController& getLedController() {
        return m_ledCtrl;
    }
};

class DroneControllerTest : public ::testing::Test {
public:
    void discovery() {
        EXPECT_EQ(tcpClient.openClient(VAR_DRONE_ADDRESS, DRONE_PORT_DISCOVERY_DEFAULT), 1);

        nlohmann::json json = {
            {CTRL_PORT_RCV, VAR_CLIENT_RCV_PORT}
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

        ASSERT_EQ(droneRcvPort, DRONE_PORT_RCV_DEFAULT);
        ASSERT_EQ(droneSendPort, DRONE_PORT_SEND_DEFAULT);
        ASSERT_EQ(maxFragmentSize, NET_FRAGMENT_SIZE_DEFAULT);
        ASSERT_EQ(maxFragmentNumber, NET_FRAGMENT_NUMBER_DEFAULT);
    }

    MockDroneController ctrl;
    NetTcp tcpClient;
protected:
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();
        EXPECT_EQ(ctrl.begin(), 1);
    }
    void TearDown() override {
        remove(CONFIG_FILE);
        ctrl.end();
        tcpClient.close();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_CLIENT_RCV_PORT = 1155;
    const int VAR_DRONE_SEND_PORT = 1156;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests Network discovery with default config
TEST_F(DroneControllerTest, DiscoveryWithDefaultConfigWorks) {
    std::thread droneProcess([this] { EXPECT_EQ(ctrl.discovery(), 1); });

    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::thread clientProcess([this] { discovery(); });

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(VAR_DRONE_ADDRESS, Config::getStringVar(DRONE_ADDRESS));
    ASSERT_EQ(VAR_CLIENT_RCV_PORT, Config::getIntVar(CTRL_PORT_RCV));
    ASSERT_EQ(ctrl.end(), 1);
}

// Tests DroneController running works
TEST_F(DroneControllerTest, RunWorks) {
    std::thread droneProcess([this] { ctrl.start(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(ctrl.isRunning());
    ASSERT_EQ(ctrl.getState(), APP_DISCOVERING);
    
    std::thread clientProcess([this] { discovery(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    clientProcess.join();
    
    ASSERT_EQ(VAR_DRONE_ADDRESS, Config::getStringVar(DRONE_ADDRESS));
    ASSERT_EQ(VAR_CLIENT_RCV_PORT, Config::getIntVar(CTRL_PORT_RCV));
    ASSERT_EQ(ctrl.getState(), APP_RUNNING);

    ASSERT_EQ(ctrl.end(), 1);

    droneProcess.join();
}