#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>
#include <chrono>

#include <PCController.h>
#include <Constants.h>
#include <net/NetTcp.h>
#include <net/NetHelper.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <nlohmann/json.hpp>
#include <PCHelper.h>

using namespace utils;
using namespace net;

class PCControllerTest : public ::testing::Test {
public:
    PCController ctrl;
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
    const int VAR_DRONE_RCV_PORT = 1555;
    const int VAR_DRONE_SEND_PORT = 1556;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

void runCtrlDiscovery(PCController* ctrl) {
    EXPECT_EQ(ctrl->discovery(), 1);
}

void runCtrlStart(PCController* ctrl) {
    EXPECT_EQ(ctrl->begin(), 1);
    ctrl->start();
}

void runDroneDiscovery(const char* discoveryAddr, int discoveryPort, 
    int clientRcvPortExp, const char* clientAddr, int droneRcvPort, 
    int droneSendPort, int maxFragmentSize, int maxFragmentNumber) {
    
    NetTcp tcpDrone;
    int clientRcvPort;
    struct sockaddr_in client;

    EXPECT_EQ(tcpDrone.openServer(discoveryAddr, discoveryPort), 1);
    EXPECT_EQ(tcpDrone.listen(client), 1);

    ASSERT_EQ(NetHelper::getIpv4Addr(client), clientAddr);

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
        {DRONE_PORT_SEND, Config::getInt(DRONE_PORT_SEND, droneSendPort)},
		{NET_FRAGMENT_SIZE, Config::getInt(NET_FRAGMENT_SIZE, maxFragmentSize)},
		{NET_FRAGMENT_NUMBER, Config::getInt(NET_FRAGMENT_NUMBER, maxFragmentNumber)}
    };
    std::string msg = json.dump();

    EXPECT_GT(tcpDrone.send(msg.c_str(), msg.length()), 0);
}

// Tests PCController discovery with default config
TEST_F(PCControllerTest, DiscoveryWithDefaultConfigWorks) {    
    std::thread droneProcess(runDroneDiscovery, VAR_DRONE_ADDRESS, 
        DRONE_PORT_DISCOVERY_DEFAULT, CTRL_PORT_RCV_DEFAULT, VAR_DRONE_ADDRESS, 
        VAR_DRONE_RCV_PORT, VAR_DRONE_SEND_PORT, MAX_FRAGMENT_SIZE, MAX_FRAGMENT_NUMBER);

     // sleep 100 milliseconds
     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess(runCtrlDiscovery, &ctrl);

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(VAR_DRONE_RCV_PORT, ctrl.getDroneRcvPort());
    ASSERT_EQ(VAR_DRONE_SEND_PORT, ctrl.getDroneSendPort());
    ASSERT_EQ(MAX_FRAGMENT_SIZE, ctrl.getMaxFragementSize());
    ASSERT_EQ(MAX_FRAGMENT_NUMBER, ctrl.getMaxFragementNumber());
}

// Tests PCController start and end
TEST_F(PCControllerTest, StartAndEndWork) {    
    std::thread droneProcess(runDroneDiscovery, VAR_DRONE_ADDRESS, 
        DRONE_PORT_DISCOVERY_DEFAULT, CTRL_PORT_RCV_DEFAULT, VAR_DRONE_ADDRESS, 
        VAR_DRONE_RCV_PORT, VAR_DRONE_SEND_PORT, MAX_FRAGMENT_SIZE, MAX_FRAGMENT_NUMBER);

    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess(runCtrlStart, &ctrl); 
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(ctrl.isRunning());

    SDL_Event sdlevent = {};
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_FALSE(ctrl.isRunning());

    droneProcess.join();
    clientProcess.join();
}

// Tests PCController start with discovering error
TEST_F(PCControllerTest, StartWithDiscoveryErrorWorks) {    
    std::thread clientProcess(runCtrlStart, &ctrl);
    ASSERT_EQ(ctrl.getState(), PC_INIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(ctrl.isRunning());
    ASSERT_EQ(ctrl.getState(), PC_ERROR);

    std::thread droneProcess(runDroneDiscovery, VAR_DRONE_ADDRESS, 
        DRONE_PORT_DISCOVERY_DEFAULT, CTRL_PORT_RCV_DEFAULT, VAR_DRONE_ADDRESS, 
        VAR_DRONE_RCV_PORT, VAR_DRONE_SEND_PORT, MAX_FRAGMENT_SIZE, MAX_FRAGMENT_NUMBER);

    // restart discovering
    SDL_Event sdlevent = {};
    sdlevent.type = SDL_KEYDOWN;
    sdlevent.key.keysym.sym = SDLK_F1;
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(ctrl.getState(), PC_RUNNING);

    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_FALSE(ctrl.isRunning());

    droneProcess.join();
    clientProcess.join();
}