#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

#include <net/NetTcp.h>
#include <net/NetHelper.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <utils/Structs.h>

#include <PCController.h>
#include <DroneController.h>
#include <EventHandler.h>
#include <controller/MotorController.h>

using namespace utils;
using namespace net;
using namespace controller;

// Mock class of the DroneController
class MockDroneController : public DroneController {
public:
    MotorController& getMotorCtroller() {
        return m_motorCtrl;
    }
};

class IntegrationTest : public ::testing::Test {
public:
    PCController pcCtrl;
    MockDroneController droneCtrl;
    std::thread droneProcess;
    std::thread pcProcess;
protected:
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{\"" << DRONE_ADDRESS << "\":\"" << VAR_DRONE_ADDRESS << "\"}";
        configFile.close();
        Config::init();

        ASSERT_EQ(droneCtrl.begin(), 1);  
        ASSERT_EQ(pcCtrl.begin(), 1);

        ASSERT_EQ(droneCtrl.getState(), APP_INIT);
        ASSERT_EQ(pcCtrl.getState(), APP_INIT);

        droneProcess = std::thread([this]{ droneCtrl.start(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        pcProcess = std::thread([this]{ pcCtrl.start(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        ASSERT_TRUE(droneCtrl.isRunning());
        ASSERT_TRUE(pcCtrl.isRunning());

        ASSERT_EQ(droneCtrl.getState(), APP_RUNNING);
        ASSERT_EQ(pcCtrl.getState(), APP_RUNNING);
    }
    void TearDown() override {
        remove(CONFIG_FILE);
        remove(KEYS_CONFIG);
        
        ASSERT_EQ(pcCtrl.end(), 1);
        ASSERT_EQ(droneCtrl.end(), 1);

        pcProcess.join();
        droneProcess.join();
    }

    const char* VAR_DRONE_ADDRESS = "127.0.0.1";
};

// Integration test for quit command
TEST_F(IntegrationTest, QuitCmdWorks) {  
    SDL_Event sdlevent = {.type = SDL_QUIT};
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(pcCtrl.isRunning());
    ASSERT_FALSE(droneCtrl.isRunning());
}

// Integration test for navigation commands
TEST_F(IntegrationTest, NavCmdsWork) {  
    MotorController& motorCtrl = droneCtrl.getMotorCtroller();
    ASSERT_TRUE(motorCtrl.isRunning());

    SDL_Event sdlevent = {};
    sdlevent.type = SDL_KEYDOWN;
    sdlevent.key.keysym.sym = SDLK_w;
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS));
    EXPECT_TRUE(motorCtrl.isOn(WHEEL_TR_FORWARD));
    EXPECT_TRUE(motorCtrl.isOn(WHEEL_TL_FORWARD));
    EXPECT_TRUE(motorCtrl.isOn(WHEEL_BR_FORWARD));
    EXPECT_TRUE(motorCtrl.isOn(WHEEL_BL_FORWARD));

    sdlevent.type = SDL_KEYUP;
    SDL_PushEvent(&sdlevent);

    std::this_thread::sleep_for(std::chrono::milliseconds(DRONE_WHEEL_MOVE_LAPS * 4));
    EXPECT_FALSE(motorCtrl.isOn(WHEEL_TR_FORWARD));
    EXPECT_FALSE(motorCtrl.isOn(WHEEL_TL_FORWARD));
    EXPECT_FALSE(motorCtrl.isOn(WHEEL_BR_FORWARD));
    EXPECT_FALSE(motorCtrl.isOn(WHEEL_BL_FORWARD));
}