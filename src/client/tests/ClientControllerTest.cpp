#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>
#include <chrono>

#include <ClientController.h>
#include <IEventHandler.h>
#include <net/NetTcp.h>
#include <net/NetHelper.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <utils/Structs.h>

using namespace utils;
using namespace net;


class EventHandlerImpl : public IEventHandler {
public:
    bool isEventDown(unsigned int eventID) override {
        return m_eventID == (int)eventID;
    }
	bool isEventPressed(unsigned int eventID) override {
        bool flag = m_eventID == (int)eventID;
        if(flag) m_eventID = -1;
        return flag;
    }
    std::string getMapping(unsigned int eventID) override {
        return "";
    }
    void press(unsigned int eventID) {
        m_eventID = (int)eventID;
    }
private:
    int m_eventID = -1;
};

class ClientControllerImpl : public ClientController {
public:
    ClientControllerImpl() : ClientController() {
        m_evHandler = &m_evHandlerImpl;
    }
    void innerRunServices() override { }
    void innerUpdateState(utils::AppState state, int error) override { }
    void press(unsigned int eventID) {
        m_evHandlerImpl.press(eventID);
    }
private:
    EventHandlerImpl m_evHandlerImpl;
};

class ClientControllerTest : public ::testing::Test {
public:
    void discovery() {
        struct sockaddr_in client;

        EXPECT_EQ(tcpDrone.openServer(VAR_DRONE_ADDRESS, DRONE_PORT_DISCOVERY_DEFAULT), 1);
        EXPECT_EQ(tcpDrone.listen(client), 1);

        ASSERT_EQ(NetHelper::getIpv4Addr(client), VAR_DRONE_ADDRESS);

        char buf[1024];
        EXPECT_GT(tcpDrone.receive(buf, 1024), 0);

        EXPECT_EQ(Config::decodeJson(std::string(buf)), 1);
        ASSERT_EQ(Config::getInt(CLIENT_PORT_RCV), CLIENT_PORT_RCV_DEFAULT);

        Config::setInt(DRONE_PORT_RCV, VAR_DRONE_RCV_PORT);
        Config::setInt(DRONE_PORT_SEND, VAR_DRONE_SEND_PORT);
        Config::setInt(NET_FRAGMENT_SIZE, MAX_FRAGMENT_SIZE);
        Config::setInt(NET_FRAGMENT_NUMBER, MAX_FRAGMENT_NUMBER);
        Config::setInt(VIDEO_FPS, VIDEO_FPS_VALUE);
        Config::setInt(VIDEO_WIDTH, VIDEO_WIDTH_VALUE);
        Config::setInt(VIDEO_HEIGHT, VIDEO_HEIGHT_VALUE);
        Config::setInt(VIDEO_FORMAT, VIDEO_FORMAT_VALUE);
        Config::setInt(VIDEO_CODEC, VIDEO_CODEC_VALUE);
        Config::setInt(AUDIO_CODEC, AUDIO_CODEC_VALUE);
        Config::setInt(AUDIO_FORMAT, AUDIO_FORMAT_VALUE);
        Config::setInt(AUDIO_SAMPLE, AUDIO_SAMPLE_VALUE);
        Config::setInt(AUDIO_BIT_RATE, AUDIO_BIT_RATE_VALUE);
        Config::setInt(AUDIO_CHANNELS, AUDIO_CHANNELS_VALUE);
        Config::setInt(LEDS_ACTIVE, LEDS_ACTIVE_VALUE);
        Config::setInt(MOTORS_ACTIVE, MOTORS_ACTIVE_VALUE);
        Config::setInt(SERVOS_ACTIVE, SERVOS_ACTIVE_VALUE);
        Config::setInt(MICRO_ACTIVE, MICRO_ACTIVE_VALUE);
        Config::setInt(CAMERA_ACTIVE, CAMERA_ACTIVE_VALUE);

        std::string msg = Config::encodeJson({
            DRONE_PORT_RCV, DRONE_PORT_SEND, NET_FRAGMENT_SIZE, NET_FRAGMENT_NUMBER,
            VIDEO_FPS, VIDEO_CODEC, VIDEO_FORMAT, VIDEO_WIDTH, VIDEO_HEIGHT, 
            AUDIO_CODEC, AUDIO_FORMAT, AUDIO_SAMPLE, AUDIO_BIT_RATE, AUDIO_CHANNELS,
            LEDS_ACTIVE, MOTORS_ACTIVE, SERVOS_ACTIVE, MICRO_ACTIVE, CAMERA_ACTIVE
        });

        EXPECT_GT(tcpDrone.send(msg.c_str(), msg.length()), 0);
    }

    ClientControllerImpl ctrl;
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
    const int VIDEO_FPS_VALUE = 30;
    const int VIDEO_WIDTH_VALUE = 200;
    const int VIDEO_HEIGHT_VALUE = 120;
    const int VIDEO_FORMAT_VALUE = 2;
    const int VIDEO_CODEC_VALUE = 13;
    const int AUDIO_CODEC_VALUE = 65897;
    const int AUDIO_FORMAT_VALUE = 2;
    const int AUDIO_SAMPLE_VALUE = 48000;
    const int AUDIO_BIT_RATE_VALUE = 158000;
    const int AUDIO_CHANNELS_VALUE = 2;
    const int LEDS_ACTIVE_VALUE = 1;
    const int MOTORS_ACTIVE_VALUE = 0;
    const int SERVOS_ACTIVE_VALUE = 1;
    const int MICRO_ACTIVE_VALUE = 0;
    const int CAMERA_ACTIVE_VALUE = 1;
};

// Tests ClientController discovery with default config
TEST_F(ClientControllerTest, DiscoveryWithDefaultConfigWorks) {    
    std::thread droneProcess([this] { discovery(); });

    // sleep 100 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::thread clientProcess([this] { EXPECT_EQ(ctrl.discovery(), 1); });

    droneProcess.join();
    clientProcess.join();

    ASSERT_EQ(VAR_DRONE_RCV_PORT, Config::getInt(DRONE_PORT_RCV));
    ASSERT_EQ(VAR_DRONE_SEND_PORT, Config::getInt(DRONE_PORT_SEND));
    ASSERT_EQ(MAX_FRAGMENT_SIZE, Config::getInt(NET_FRAGMENT_SIZE));
    ASSERT_EQ(MAX_FRAGMENT_NUMBER, Config::getInt(NET_FRAGMENT_NUMBER));
    ASSERT_EQ(VIDEO_FPS_VALUE, Config::getInt(VIDEO_FPS));
    ASSERT_EQ(VIDEO_WIDTH_VALUE, Config::getInt(VIDEO_WIDTH));
    ASSERT_EQ(VIDEO_HEIGHT_VALUE, Config::getInt(VIDEO_HEIGHT));
    ASSERT_EQ(VIDEO_FORMAT_VALUE, Config::getInt(VIDEO_FORMAT));

    ASSERT_EQ(AUDIO_CODEC_VALUE, Config::getInt(AUDIO_CODEC));
    ASSERT_EQ(AUDIO_FORMAT_VALUE, Config::getInt(AUDIO_FORMAT));
    ASSERT_EQ(AUDIO_SAMPLE_VALUE, Config::getInt(AUDIO_SAMPLE));
    ASSERT_EQ(AUDIO_BIT_RATE_VALUE, Config::getInt(AUDIO_BIT_RATE));
    ASSERT_EQ(AUDIO_CHANNELS_VALUE, Config::getInt(AUDIO_CHANNELS));

    ASSERT_EQ(LEDS_ACTIVE_VALUE, Config::getInt(LEDS_ACTIVE));
    ASSERT_EQ(MOTORS_ACTIVE_VALUE, Config::getInt(MOTORS_ACTIVE));
    ASSERT_EQ(SERVOS_ACTIVE_VALUE, Config::getInt(SERVOS_ACTIVE));
    ASSERT_EQ(MICRO_ACTIVE_VALUE, Config::getInt(MICRO_ACTIVE));
    ASSERT_EQ(CAMERA_ACTIVE_VALUE, Config::getInt(CAMERA_ACTIVE));
}

// Tests ClientController end
TEST_F(ClientControllerTest, EndWorks) {      
    std::thread clientProcess([this] { ctrl.start(); });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(ctrl.isRunning());

    ASSERT_EQ(ctrl.end(), 1);
    ASSERT_FALSE(ctrl.isRunning());

    clientProcess.join();
}

// Tests ClientController start and end
TEST_F(ClientControllerTest, StartAndEndWork) {    
    std::thread droneProcess([this] { discovery(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread clientProcess([this] { ctrl.start(); });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(ctrl.isRunning());

    ctrl.press(ClientEvent::QUIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_FALSE(ctrl.isRunning());

    droneProcess.join();
    clientProcess.join();
}

// Tests ClientController start with discovering error
TEST_F(ClientControllerTest, StartWithDiscoveryErrorWorks) {    
    std::thread clientProcess([this] { ctrl.start(); });
    ASSERT_EQ(ctrl.getState(), APP_INIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(ctrl.isRunning());
    ASSERT_EQ(ctrl.getState(), APP_ERROR);

    std::thread droneProcess([this] { discovery(); });

    // restart discovering
    ctrl.press(ClientEvent::DISCOVER);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(ctrl.getState(), APP_RUNNING);

    ctrl.press(ClientEvent::QUIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_FALSE(ctrl.isRunning());

    droneProcess.join();
    clientProcess.join();
}