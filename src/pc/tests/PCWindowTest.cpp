#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>

#include <PCWindow.h>
#include <EventHandler.h>
#include <Constants.h>
#include <utils/Constants.h>
#include <utils/Config.h>

using namespace utils;

class PCWindowTest : public ::testing::Test {
public:
    PCWindowTest() : evHandler(window.getEventHandler()) {}
    PCWindow window;
    EventHandler &evHandler;
protected:
    void SetUp() override {
        Config::setInt(VIDEO_WIDTH, VIDEO_WIDTH_DEFAULT);
        Config::setInt(VIDEO_HEIGHT, VIDEO_HEIGHT_DEFAULT);
        Config::setInt(VIDEO_FPS, VIDEO_FPS_DEFAULT);
        Config::setString(VIDEO_PIX_FORMAT, VIDEO_PIX_FORMAT_DEFAULT);
    }
    void TearDown() override {
        remove(CONFIG_FILE);
    }
};

// Tests PCWindow begin with default config
TEST_F(PCWindowTest, BeginWithDefaultConfigWorks) {
    ASSERT_EQ(window.begin(), 1);
    ASSERT_EQ(window.end(), 1);
    ASSERT_FALSE(window.isRunning());
}

// Tests PCWindow start
TEST_F(PCWindowTest, StartServiceWorks) {
    ASSERT_EQ(window.begin(), 1);
    window.start();
    ASSERT_TRUE(window.isRunning());
    ASSERT_EQ(window.end(), 1);
    ASSERT_FALSE(window.isRunning());
}

// Tests PCWindow run
TEST_F(PCWindowTest, RunServiceWorks) {
    ASSERT_EQ(window.begin(), 1);
    window.start();
    ASSERT_TRUE(window.isRunning());

    SDL_Event sdlevent = {};
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);

    window.run();

    ASSERT_TRUE(evHandler.isEventPressed(ClientEvent::QUIT));
    ASSERT_EQ(window.end(), 1);
}