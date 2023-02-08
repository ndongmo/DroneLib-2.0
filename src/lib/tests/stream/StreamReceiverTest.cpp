#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net/NetSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <stream/StreamReceiver.h>

using namespace utils;
using namespace net;
using namespace stream;

// Mock class of the StreamReceiver
class MockStreamReceiver : public StreamReceiver {
public:
    int getOldFrame() { return m_oldFrame; }
    int getCurrentFrame() { return m_currentFrame; }
    bool hasNewBufferFrame() { return m_newFrame; }
    bool canAck() { return m_ackStreamFragment; }

    StreamPool& getPool() { return m_pool; }
    AvStreamFrame& getFrame() { return m_frame; }
};

/**
 * @brief StreamReceiver Test. It need 
 * a capture device (camera) file at device/video0
 */
class StreamReceiverTest : public ::testing::Test {
public:
    void testReceivedFragment(StreamFragment &sf, int sentValue) {
        NetHelper::writeUInt32(sentValue, sf.fragmentData, 0);
        UINT8 *ack = videoRcv.newFragment(sf);
        int index = videoRcv.getFrame().fragmentNumber * videoRcv.getFrame().fragmentSize;
        int rcvValue = NetHelper::readUInt32(videoRcv.getFrame().frame, index);

        EXPECT_EQ(videoRcv.canAck(), ack != nullptr);
        EXPECT_EQ(videoRcv.getFrame().fragmentNumber, (int)sf.fragmentNumber);
        EXPECT_EQ(videoRcv.getFrame().frameNumber, (int)sf.frameNumber);
        EXPECT_EQ(videoRcv.getFrame().fragmentSize, (int)sf.fragmentSize);
        EXPECT_EQ(videoRcv.getFrame().frameFlags, sf.frameFlags);
        EXPECT_EQ(videoRcv.getFrame().frameSize, (int)sf.frameSize);
        EXPECT_EQ(sentValue, rcvValue);
    }
    MockStreamReceiver videoRcv;
protected:
    void SetUp() override {
        
    }
    void TearDown() override {
        videoRcv.end();
    }
};

// Tests StreamReceiver process
TEST_F(StreamReceiverTest, RunWorks) {      
    EXPECT_EQ(videoRcv.begin(), 1);

    videoRcv.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(videoRcv.isRunning());
    ASSERT_FALSE(videoRcv.hasNewBufferFrame());
    EXPECT_EQ(videoRcv.getCurrentFrame(), -1);
    EXPECT_EQ(videoRcv.getCurrentFrame(), videoRcv.getOldFrame());
    EXPECT_EQ(videoRcv.getPool().getSize(videoRcv.getCurrentFrame()), 0);
    EXPECT_EQ(videoRcv.getPool().getData(videoRcv.getCurrentFrame()), nullptr);

    EXPECT_EQ(videoRcv.end(), 1);
    ASSERT_FALSE(videoRcv.isRunning());
}

// Tests StreamReceiver newFragment
TEST_F(StreamReceiverTest, newFragmentWorks) {      
    EXPECT_EQ(videoRcv.begin(), 1);

    videoRcv.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(videoRcv.isRunning());
    ASSERT_FALSE(videoRcv.hasNewBufferFrame());
    EXPECT_EQ(videoRcv.getCurrentFrame(), -1);
    EXPECT_EQ(videoRcv.getCurrentFrame(), videoRcv.getOldFrame());
    EXPECT_EQ(videoRcv.getPool().getSize(videoRcv.getCurrentFrame()), 0);
    EXPECT_EQ(videoRcv.getPool().getData(videoRcv.getCurrentFrame()), nullptr);

    StreamFragment sf {
        .frameNumber = 1,
        .fragmentNumber = 0,
        .frameSize = 20,
        .fragmentSize = 10,
        .fragmentData = new UINT8[10]()
    };

    testReceivedFragment(sf, 75);
    sf.fragmentNumber++;
    testReceivedFragment(sf, 897);
    sf.frameNumber++;
    sf.fragmentNumber = 0;
    testReceivedFragment(sf, 256);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(videoRcv.getCurrentFrame(), 0);
    EXPECT_EQ(videoRcv.getPool().getSize(videoRcv.getCurrentFrame()), videoRcv.getFrame().frameSize);
    EXPECT_EQ((int)NetHelper::readUInt32(videoRcv.getPool().getData(videoRcv.getCurrentFrame()), 0), 75);
    EXPECT_EQ((int)NetHelper::readUInt32(videoRcv.getPool().getData(videoRcv.getCurrentFrame()), videoRcv.getFrame().fragmentSize), 897);
    
    ASSERT_FALSE(videoRcv.hasNewBufferFrame());

    videoRcv.updateFrame();

    EXPECT_EQ(videoRcv.getCurrentFrame(), videoRcv.getOldFrame());
    ASSERT_TRUE(videoRcv.hasNewBufferFrame());
    ASSERT_TRUE(videoRcv.getSize() > 0);
    ASSERT_TRUE(videoRcv.getData() != nullptr);
    
    EXPECT_EQ(videoRcv.end(), 1);
    ASSERT_FALSE(videoRcv.isRunning());
    delete[] sf.fragmentData;
}