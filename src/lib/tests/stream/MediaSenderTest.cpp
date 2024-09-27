#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net/NetSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Helper.h>
#include <stream/MediaSender.h>

using namespace utils;
using namespace net;
using namespace stream;

// Mock class of the StreamSender
class MockMediaSender : public MediaSender {
public:
    MockMediaSender() : MediaSender(netSender) { }
    AVIOContext* getHttpClient() { return m_http_client; }
    AVFormatContext* getOutputFormatCtx() { return m_ofmt_ctx; }
    VideoSender& getVideoSender() { return m_videoSender; }
    AudioSender& getAudioSender() { return m_audioSender; }

protected:
    NetSender netSender;
};

class MediaSenderTest : public ::testing::Test {
public:
    MockMediaSender mediaSender;
    const char* SERVER_ADDRESS = "127.0.0.1";
protected:
    void SetUp() override {        
        Config::init();

        Config::setIntDefault(VIDEO_FPS, VIDEO_FPS_DEFAULT);
        Config::setIntDefault(VIDEO_WIDTH, VIDEO_WIDTH_DEFAULT);
        Config::setIntDefault(VIDEO_HEIGHT, VIDEO_HEIGHT_DEFAULT);
        Config::setStringDefault(VIDEO_ENCODER, VIDEO_ENCODER_DEFAULT);
        Config::setStringDefault(VIDEO_PIX_FORMAT, VIDEO_PIX_FORMAT_DEFAULT);
        Config::setStringDefault(VIDEO_DEVICE, VIDEO_DEVICE_DEFAULT);
        Config::setStringDefault(VIDEO_INPUT_FORMAT, VIDEO_INPUT_FORMAT_DEFAULT);

        Config::setIntDefault(AUDIO_CHANNELS, AUDIO_CHANNELS_DEFAULT);
        Config::setIntDefault(AUDIO_BIT_RATE, AUDIO_BIT_RATE_DEFAULT);
        Config::setIntDefault(AUDIO_SAMPLE, AUDIO_SAMPLE_DEFAULT);
        Config::setIntDefault(AUDIO_NB_SAMPLES, AUDIO_NB_SAMPLES_DEFAULT);
        Config::setIntDefault(AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_FORMAT_DEFAULT);
        Config::setStringDefault(AUDIO_ENCODER, "pcm_s16le");
        Config::setStringDefault(AUDIO_DEVICE, AUDIO_DEVICE_DEFAULT);
        Config::setStringDefault(AUDIO_INPUT_FORMAT, AUDIO_INPUT_FORMAT_DEFAULT);

        Config::setIntDefault(STREAM_MODE, STREAM_MODE_FILE);
        Config::setStringDefault(STREAM_PROTOCOL, STREAM_PROTOCOL_DEFAULT);
	    Config::setStringDefault(STREAM_OUTPUT_FORMAT, STREAM_OUTPUT_FORMAT_DEFAULT);
        Config::setIntDefault(DRONE_PORT_STREAM, 4443);
        Config::setStringDefault(DRONE_ADDRESS, SERVER_ADDRESS);
        Config::setStringDefault(STREAM_OUT_FILE_ADDRESS, Helper::generateStreamUrl());

        EXPECT_EQ(mediaSender.getOutputFormatCtx(), nullptr);
    }
    void TearDown() override {
        mediaSender.end();
        EXPECT_EQ(mediaSender.getOutputFormatCtx(), nullptr);
    }
};

// Tests MediaSender stream when no media is active
TEST_F(MediaSenderTest, TryStreamWhenNotActive) {  
    Config::setInt(MICRO_ACTIVE, 0);
    Config::setInt(CAMERA_ACTIVE, 0);

    ASSERT_FALSE(mediaSender.isActive());
    EXPECT_EQ(mediaSender.begin(), -1);

    EXPECT_EQ(mediaSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(mediaSender.getHttpClient(), nullptr);

    mediaSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_FALSE(mediaSender.isRunning());
    ASSERT_FALSE(mediaSender.getVideoSender().isRunning());
    ASSERT_FALSE(mediaSender.getAudioSender().isRunning());
    EXPECT_EQ(mediaSender.end(), 1);
    ASSERT_FALSE(mediaSender.isRunning());
}

// Tests MediaSender udp video stream
TEST_F(MediaSenderTest, RunUDPVideoStreamWorks) {  
    Config::setInt(MICRO_ACTIVE, 0);
    Config::setInt(CAMERA_ACTIVE, 1);

    ASSERT_TRUE(mediaSender.isActive());
    ASSERT_TRUE(mediaSender.getVideoSender().isActive());
    ASSERT_FALSE(mediaSender.getAudioSender().isActive());

    EXPECT_EQ(mediaSender.begin(), 1);

    EXPECT_NE(mediaSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(mediaSender.getHttpClient(), nullptr);

    mediaSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(mediaSender.isRunning());
    ASSERT_TRUE(mediaSender.getVideoSender().isRunning());
    ASSERT_FALSE(mediaSender.getAudioSender().isRunning());
    EXPECT_EQ(mediaSender.end(), 1);
    ASSERT_FALSE(mediaSender.isRunning());
    ASSERT_FALSE(mediaSender.getVideoSender().isRunning());
}

// Tests MediaSender udp audio stream
TEST_F(MediaSenderTest, RunUDPAudioStreamWorks) {  
    Config::setInt(MICRO_ACTIVE, 1);
    Config::setInt(CAMERA_ACTIVE, 0);

    ASSERT_TRUE(mediaSender.isActive());
    ASSERT_FALSE(mediaSender.getVideoSender().isActive());
    ASSERT_TRUE(mediaSender.getAudioSender().isActive());

    EXPECT_EQ(mediaSender.begin(), 1);

    EXPECT_NE(mediaSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(mediaSender.getHttpClient(), nullptr);

    mediaSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(mediaSender.isRunning());
    ASSERT_FALSE(mediaSender.getVideoSender().isRunning());
    ASSERT_TRUE(mediaSender.getAudioSender().isRunning());
    EXPECT_EQ(mediaSender.end(), 1);
    ASSERT_FALSE(mediaSender.isRunning());
    ASSERT_FALSE(mediaSender.getAudioSender().isRunning());
}

// Tests MediaSender udp all media stream
TEST_F(MediaSenderTest, RunUDPAllMediaStreamWorks) {  
    Config::setInt(MICRO_ACTIVE, 1);
    Config::setInt(CAMERA_ACTIVE, 1);

    ASSERT_TRUE(mediaSender.isActive());
    ASSERT_TRUE(mediaSender.getVideoSender().isActive());
    ASSERT_TRUE(mediaSender.getAudioSender().isActive());

    EXPECT_EQ(mediaSender.begin(), 1);

    EXPECT_NE(mediaSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(mediaSender.getHttpClient(), nullptr);

    mediaSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(mediaSender.isRunning());
    ASSERT_TRUE(mediaSender.getVideoSender().isRunning());
    ASSERT_TRUE(mediaSender.getAudioSender().isRunning());
    EXPECT_EQ(mediaSender.end(), 1);
    ASSERT_FALSE(mediaSender.isRunning());
    ASSERT_FALSE(mediaSender.getVideoSender().isRunning());
    ASSERT_FALSE(mediaSender.getAudioSender().isRunning());
}

// Tests MediaSender tcp with http can stop
TEST_F(MediaSenderTest, RunTCPStopWorks) {  
    Config::setInt(MICRO_ACTIVE, 1);
    Config::setInt(CAMERA_ACTIVE, 1);
    Config::setString(STREAM_PROTOCOL, "http");
    Config::setString(STREAM_OUT_FILE_ADDRESS, Helper::generateStreamUrl());

    ASSERT_TRUE(mediaSender.isActive());
    ASSERT_TRUE(mediaSender.getVideoSender().isActive());
    ASSERT_TRUE(mediaSender.getAudioSender().isActive());

    EXPECT_EQ(mediaSender.begin(), 1);

    EXPECT_NE(mediaSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(mediaSender.getHttpClient(), nullptr);

    mediaSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ASSERT_FALSE(mediaSender.isRunning());
    ASSERT_FALSE(mediaSender.getVideoSender().isRunning());
    ASSERT_FALSE(mediaSender.getAudioSender().isRunning());
    
    mediaSender.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    ASSERT_FALSE(mediaSender.isRunning());
    ASSERT_FALSE(mediaSender.getVideoSender().isRunning());
    ASSERT_FALSE(mediaSender.getAudioSender().isRunning());

    EXPECT_EQ(mediaSender.end(), 1);
}