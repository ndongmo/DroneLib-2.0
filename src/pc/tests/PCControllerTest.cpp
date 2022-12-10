#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

#include <PCController.h>
#include <Constants.h>
#include <net/NetTcp.h>
#include <net/NetHelper.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <utils/Structs.h>

using namespace utils;
using namespace net;

class PCControllerTest : public ::testing::Test {
public:
    void discovery() {
        int clientRcvPort;
        struct sockaddr_in client;

        EXPECT_EQ(tcpDrone.openServer(VAR_DRONE_ADDRESS, DRONE_PORT_DISCOVERY_DEFAULT), 1);
        EXPECT_EQ(tcpDrone.listen(client), 1);

        ASSERT_EQ(NetHelper::getIpv4Addr(client), VAR_DRONE_ADDRESS);

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
        ASSERT_EQ(clientRcvPort, CTRL_PORT_RCV_DEFAULT);

        json = {
            {DRONE_PORT_RCV, Config::getInt(DRONE_PORT_RCV, VAR_DRONE_RCV_PORT)},
            {DRONE_PORT_SEND, Config::getInt(DRONE_PORT_SEND, VAR_DRONE_SEND_PORT)},
            {NET_FRAGMENT_SIZE, Config::getInt(NET_FRAGMENT_SIZE, MAX_FRAGMENT_SIZE)},
            {NET_FRAGMENT_NUMBER, Config::getInt(NET_FRAGMENT_NUMBER, MAX_FRAGMENT_NUMBER)}
        };
        std::string msg = json.dump();
        EXPECT_GT(tcpDrone.send(msg.c_str(), msg.length()), 0);
    }

    PCController ctrl;
    NetTcp tcpDrone;
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
        tcpDrone.close();
        EXPECT_EQ(ctrl.end(), 1);
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
    const int VAR_DRONE_RCV_PORT = 1515;
    const int VAR_DRONE_SEND_PORT = 1516;
    const int MAX_FRAGMENT_SIZE = 800;
    const int MAX_FRAGMENT_NUMBER = 256;
};

// Tests PCController discovery with default config
TEST_F(PCControllerTest, DiscoveryWithDefaultConfigWorks) {    
    std::thread droneProcess([this] { discovery(); });

    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::thread clientProcess([this] { EXPECT_EQ(ctrl.discovery(), 1); });

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(VAR_DRONE_RCV_PORT, ctrl.getDroneRcvPort());
    ASSERT_EQ(VAR_DRONE_SEND_PORT, ctrl.getDroneSendPort());
    ASSERT_EQ(MAX_FRAGMENT_SIZE, ctrl.getMaxFragementSize());
    ASSERT_EQ(MAX_FRAGMENT_NUMBER, ctrl.getMaxFragementNumber());
}

// Tests PCController end
TEST_F(PCControllerTest, EndWorks) {      
    std::thread clientProcess([this] { ctrl.start(); });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(ctrl.isRunning());

    ASSERT_EQ(ctrl.end(), 1);
    ASSERT_FALSE(ctrl.isRunning());

    clientProcess.join();
}

// Tests PCController start and end
TEST_F(PCControllerTest, StartAndEndWork) {    
    std::thread droneProcess([this] { discovery(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess([this] { ctrl.start(); });
    
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
    std::thread clientProcess([this] { ctrl.start(); });
    ASSERT_EQ(ctrl.getState(), APP_INIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(ctrl.isRunning());
    ASSERT_EQ(ctrl.getState(), APP_ERROR);

    std::thread droneProcess([this] { discovery(); });

    // restart discovering
    SDL_Event sdlevent = {};
    sdlevent.type = SDL_KEYDOWN;
    sdlevent.key.keysym.sym = SDLK_F1;
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(ctrl.getState(), APP_RUNNING);

    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_FALSE(ctrl.isRunning());

    droneProcess.join();
    clientProcess.join();
}