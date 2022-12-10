#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <thread>

#include <PCWindow.h>
#include <EventHandler.h>
#include <Constants.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <nlohmann/json.hpp>

using namespace utils;

class PCWindowTest : public ::testing::Test {
protected:
    void TearDown() override {
        remove(CONFIG_FILE);
    }
};

// Tests PCWindow begin with default config
TEST_F(PCWindowTest, BeginWithDefaultConfigWorks) {
    EventHandler evHandler;
    PCWindow window(evHandler);

    ASSERT_EQ(window.begin(), 1);
    ASSERT_EQ(window.end(), 1);
    ASSERT_FALSE(window.isRunning());
}

// Tests PCWindow start
TEST_F(PCWindowTest, StartServiceWorks) {
    EventHandler evHandler;
    PCWindow window(evHandler);

    ASSERT_EQ(window.begin(), 1);
    window.start();
    ASSERT_TRUE(window.isRunning());
    ASSERT_EQ(window.end(), 1);
    ASSERT_FALSE(window.isRunning());
}

// Tests PCWindow run
TEST_F(PCWindowTest, RunServiceWorks) {
    EventHandler evHandler;
    PCWindow window(evHandler);

    ASSERT_EQ(window.begin(), 1);
    window.start();
    ASSERT_TRUE(window.isRunning());

    SDL_Event sdlevent = {};
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);

    window.run();

    ASSERT_TRUE(evHandler.isEventPressed(CtrlEvent::QUIT));
    ASSERT_EQ(window.end(), 1);
}