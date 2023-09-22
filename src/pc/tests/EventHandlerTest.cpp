#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <EventHandler.h>

class EventHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        
    }

    void TearDown() override {
        remove(KEYS_CONFIG);
    }
};

// Tests EventHandler save/load config
TEST_F(EventHandlerTest, SaveAndLoadConfigWork) {
    EventHandler eh, eh2;
    eh.addEvent(ClientEvent::QUIT, SDLK_ESCAPE, Joystick::START);
    eh.addEvent(ClientEvent::GO_UP, SDLK_w, Joystick::AXE1_UP);
    eh.saveConfig();

    EXPECT_TRUE(eh.eventExists(ClientEvent::QUIT));
    EXPECT_TRUE(eh.eventExists(ClientEvent::GO_UP));
    
    EXPECT_TRUE(eh2.loadConfig());
    EXPECT_TRUE(eh2.eventExists(ClientEvent::QUIT));
    EXPECT_TRUE(eh2.eventExists(ClientEvent::GO_UP));
}

// Tests EventHandler press events
TEST_F(EventHandlerTest, PressEventsWork) {
    EventHandler eh;
    eh.addEvent(ClientEvent::QUIT, SDLK_ESCAPE, Joystick::START);
    eh.pressKey(SDLK_ESCAPE);

    EXPECT_TRUE(eh.isEventDown(ClientEvent::QUIT));
    EXPECT_TRUE(eh.isEventDown(ClientEvent::QUIT));
    eh.releaseEvent(ClientEvent::QUIT);
    EXPECT_FALSE(eh.isEventDown(ClientEvent::QUIT));

    eh.pressKey(SDLK_ESCAPE);
    EXPECT_TRUE(eh.isEventPressed(ClientEvent::QUIT));
    EXPECT_FALSE(eh.isEventPressed(ClientEvent::QUIT));
    eh.releaseEvent(ClientEvent::QUIT);
    EXPECT_FALSE(eh.isEventDown(ClientEvent::QUIT));    
}