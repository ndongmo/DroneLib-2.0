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
    eh.addEvent(CtrlEvent::QUIT, SDLK_ESCAPE, Joystick::START);
    eh.addEvent(CtrlEvent::GO_UP, SDLK_w, Joystick::AXE1_UP);
    eh.saveConfig();

    EXPECT_TRUE(eh.eventExists(CtrlEvent::QUIT));
    EXPECT_TRUE(eh.eventExists(CtrlEvent::GO_UP));
    
    EXPECT_TRUE(eh2.loadConfig());
    EXPECT_TRUE(eh2.eventExists(CtrlEvent::QUIT));
    EXPECT_TRUE(eh2.eventExists(CtrlEvent::GO_UP));
}

// Tests EventHandler press events
TEST_F(EventHandlerTest, PressEventsWork) {
    EventHandler eh;
    eh.addEvent(CtrlEvent::QUIT, SDLK_ESCAPE, Joystick::START);
    eh.pressKey(SDLK_ESCAPE);

    EXPECT_TRUE(eh.isEventDown(CtrlEvent::QUIT));
    EXPECT_TRUE(eh.isEventDown(CtrlEvent::QUIT));
    eh.releaseEvent(CtrlEvent::QUIT);
    EXPECT_FALSE(eh.isEventDown(CtrlEvent::QUIT));

    eh.pressKey(SDLK_ESCAPE);
    EXPECT_TRUE(eh.isEventPressed(CtrlEvent::QUIT));
    EXPECT_FALSE(eh.isEventPressed(CtrlEvent::QUIT));
    eh.releaseEvent(CtrlEvent::QUIT);
    EXPECT_FALSE(eh.isEventDown(CtrlEvent::QUIT));    
}