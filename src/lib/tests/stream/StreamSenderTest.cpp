#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net/NetSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <stream/StreamSender.h>

using namespace utils;
using namespace net;
using namespace stream;

// Mock class of the StreamSender
class MockStreamSender : public StreamSender {
public:
    MockStreamSender() : StreamSender(netSender) {
        Config::init();
        Config::setStringDefault(VIDEO_DEVICE, VIDEO_DEVICE_DEFAULT);
	    Config::setStringDefault(VIDEO_INPUT_FORMAT, VIDEO_INPUT_FORMAT_DEFAULT);

        m_streamID = NS_ID_STREAM_VIDEO;
	    m_mediaType = AVMEDIA_TYPE_VIDEO;
        m_filename = Config::getString(VIDEO_DEVICE);
	    m_formatname = Config::getString(VIDEO_INPUT_FORMAT);
    }
    int getStreamID() { return m_streamID; }
    int getFrameIndex() { return m_frameIndex; }
    std::string getFileName() { return m_filename; }
    AVMediaType getMediaType() { return m_mediaType; }

protected:
    void sendPacket() override {
        ASSERT_TRUE(m_packet->pts > 0);
        ASSERT_TRUE(m_packet->data != nullptr);
        ASSERT_TRUE(m_packet->size > 0);
    }
    NetSender netSender;
};

class StreamSenderTest : public ::testing::Test {
public:
    MockStreamSender streamSender;
protected:
    void SetUp() override {
        EXPECT_EQ(streamSender.getFrameIndex(), 0);
    }
    void TearDown() override {
        streamSender.end();
    }
};

// Tests StreamSender process
TEST_F(StreamSenderTest, RunWorks) {      
    EXPECT_EQ(streamSender.begin(), 1);

    streamSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(streamSender.isRunning());
    EXPECT_EQ(streamSender.end(), 1);
    ASSERT_FALSE(streamSender.isRunning());
}