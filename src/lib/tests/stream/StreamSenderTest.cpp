#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net/NetSender.h>
#include <utils/Constants.h>
#include <utils/Config.h>
#include <utils/Logger.h>
#include <utils/Helper.h>
#include <stream/VideoSender.h>
#include <stream/AudioSender.h>

using namespace utils;
using namespace net;
using namespace stream;

// Mock class of the StreamSender
class MockVideoSender : public VideoSender {
public:
    MockVideoSender() : VideoSender(netSender) {
        ON_CALL(*this, initDecoderCtx()).WillByDefault([this]() {
            VideoSender::initDecoderCtx();
        });
        ON_CALL(*this, initEncoderCtx()).WillByDefault([this]() {
            VideoSender::initEncoderCtx();
        });
        ON_CALL(*this, initFilterCtx()).WillByDefault([this]() {
            return VideoSender::initFilterCtx();
        });
    }
    int getFrameIndex() { return m_frameIndex; }
    AVFormatContext* getInputFormatCtx() { return m_ifmt_ctx; }
    AVFormatContext* getOutputFormatCtx() { return m_ofmt_ctx; }
    FilteringContext& getFilterCtx() { return m_filter_ctx; }
    AVCodecContext* getEncoderCtx() { return m_encoder_ctx; }
    AVCodecContext* getDecoderCtx() { return m_decoder_ctx; }
    const AVCodec* getEncoder() { return m_encoder; }
    const AVCodec* getDecoder() { return m_decoder; }
    AVStream* getInputStream() { return m_istream; }
    AVStream* getOutputStream() { return m_ostream; }

    MOCK_METHOD(void, initDecoderCtx, (), (override));
    MOCK_METHOD(void, initEncoderCtx, (), (override));
    MOCK_METHOD(int, initFilterCtx, (), (override));

protected:
    void sendPacket(AVPacket *packet) override {
        ASSERT_TRUE(packet->pts > 0);
        ASSERT_TRUE(packet->data != nullptr);
        ASSERT_TRUE(packet->size > 0);
    }
    NetSender netSender;
};

// Mock class of the StreamSender
class MockAudioSender : public AudioSender {
public:
    MockAudioSender() : AudioSender(netSender) {
        ON_CALL(*this, initDecoderCtx()).WillByDefault([this]() {
            AudioSender::initDecoderCtx();
        });
        ON_CALL(*this, initEncoderCtx()).WillByDefault([this]() {
            AudioSender::initEncoderCtx();
        });
        ON_CALL(*this, initFilterCtx()).WillByDefault([this]() {
            return AudioSender::initFilterCtx();
        });
    }
    int getFrameIndex() { return m_frameIndex; }
    AVFormatContext* getInputFormatCtx() { return m_ifmt_ctx; }
    AVFormatContext* getOutputFormatCtx() { return m_ofmt_ctx; }
    FilteringContext& getFilterCtx() { return m_filter_ctx; }
    AVCodecContext* getEncoderCtx() { return m_encoder_ctx; }
    AVCodecContext* getDecoderCtx() { return m_decoder_ctx; }
    const AVCodec* getEncoder() { return m_encoder; }
    const AVCodec* getDecoder() { return m_decoder; }
    AVStream* getInputStream() { return m_istream; }
    AVStream* getOutputStream() { return m_ostream; }

    MOCK_METHOD(void, initDecoderCtx, (), (override));
    MOCK_METHOD(void, initEncoderCtx, (), (override));
    MOCK_METHOD(int, initFilterCtx, (), (override));

protected:
    void sendPacket(AVPacket *packet) override {
        ASSERT_TRUE(packet->pts > 0);
        ASSERT_TRUE(packet->data != nullptr);
        ASSERT_TRUE(packet->size > 0);
    }
    NetSender netSender;
};

class VideoSenderTest : public ::testing::Test {
public:
    MockVideoSender videoSender;
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

        Config::setIntDefault(STREAM_MODE, STREAM_MODE_DEFAULT);
        Config::setStringDefault(STREAM_PROTOCOL, STREAM_PROTOCOL_DEFAULT);
	    Config::setStringDefault(STREAM_OUTPUT_FORMAT, STREAM_OUTPUT_FORMAT_DEFAULT);
        Config::setIntDefault(DRONE_PORT_STREAM, 4443);
        Config::setStringDefault(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);
        Config::setStringDefault(STREAM_OUT_FILE_ADDRESS, Helper::generateStreamUrl());

        EXPECT_EQ(videoSender.getFrameIndex(), 0);
        checkNull();
    }
    void TearDown() override {
        videoSender.end();
        checkNull();
    }
    void checkNull() {
        EXPECT_EQ(videoSender.getInputFormatCtx(), nullptr);
        EXPECT_EQ(videoSender.getOutputFormatCtx(), nullptr);
        EXPECT_EQ(videoSender.getDecoderCtx(), nullptr);
        EXPECT_EQ(videoSender.getEncoderCtx(), nullptr);
        EXPECT_EQ(videoSender.getDecoder(), nullptr);
        EXPECT_EQ(videoSender.getEncoder(), nullptr);
        EXPECT_EQ(videoSender.getInputStream(), nullptr);
        EXPECT_EQ(videoSender.getOutputStream(), nullptr);
        EXPECT_EQ(videoSender.getFilterCtx().filter_graph, nullptr);
        EXPECT_EQ(videoSender.getFilterCtx().buffersink_ctx, nullptr);
        EXPECT_EQ(videoSender.getFilterCtx().buffersrc_ctx, nullptr);
        EXPECT_EQ(videoSender.getFilterCtx().enc_pkt, nullptr);
        EXPECT_EQ(videoSender.getFilterCtx().filtered_frame, nullptr);
    }
};

class AudioSenderTest : public ::testing::Test {
public:
    MockAudioSender audioSender;
protected:
    void SetUp() override {        
        Config::init();

        Config::setIntDefault(AUDIO_CHANNELS, AUDIO_CHANNELS_DEFAULT);
        Config::setIntDefault(AUDIO_BIT_RATE, AUDIO_BIT_RATE_DEFAULT);
        Config::setIntDefault(AUDIO_SAMPLE, AUDIO_SAMPLE_DEFAULT);
        Config::setIntDefault(AUDIO_NB_SAMPLES, AUDIO_NB_SAMPLES_DEFAULT);
        Config::setIntDefault(AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_FORMAT_DEFAULT);
        Config::setStringDefault(AUDIO_ENCODER, "pcm_s16le");
        Config::setStringDefault(AUDIO_DEVICE, AUDIO_DEVICE_DEFAULT);
        Config::setStringDefault(AUDIO_INPUT_FORMAT, AUDIO_INPUT_FORMAT_DEFAULT);

        Config::setIntDefault(STREAM_MODE, STREAM_MODE_DEFAULT);
        Config::setStringDefault(STREAM_PROTOCOL, STREAM_PROTOCOL_DEFAULT);
	    Config::setStringDefault(STREAM_OUTPUT_FORMAT, STREAM_OUTPUT_FORMAT_DEFAULT);
        Config::setIntDefault(DRONE_PORT_STREAM, 4443);
        Config::setStringDefault(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);
        Config::setStringDefault(STREAM_OUT_FILE_ADDRESS, Helper::generateStreamUrl());

        EXPECT_EQ(audioSender.getFrameIndex(), 0);
        checkNull();
    }
    void TearDown() override {
        audioSender.end();
        checkNull();
    }
    void checkNull() {
        EXPECT_EQ(audioSender.getInputFormatCtx(), nullptr);
        EXPECT_EQ(audioSender.getOutputFormatCtx(), nullptr);
        EXPECT_EQ(audioSender.getDecoderCtx(), nullptr);
        EXPECT_EQ(audioSender.getEncoderCtx(), nullptr);
        EXPECT_EQ(audioSender.getDecoder(), nullptr);
        EXPECT_EQ(audioSender.getEncoder(), nullptr);
        EXPECT_EQ(audioSender.getInputStream(), nullptr);
        EXPECT_EQ(audioSender.getOutputStream(), nullptr);
        EXPECT_EQ(audioSender.getFilterCtx().filter_graph, nullptr);
        EXPECT_EQ(audioSender.getFilterCtx().buffersink_ctx, nullptr);
        EXPECT_EQ(audioSender.getFilterCtx().buffersrc_ctx, nullptr);
        EXPECT_EQ(audioSender.getFilterCtx().enc_pkt, nullptr);
        EXPECT_EQ(audioSender.getFilterCtx().filtered_frame, nullptr);
    }
};

// Tests VideoSender raw stream mode
TEST_F(VideoSenderTest, RunRawVideoStreamMode) {   
    EXPECT_CALL(videoSender, initDecoderCtx()).Times(1);
    EXPECT_CALL(videoSender, initEncoderCtx()).Times(0);
    EXPECT_CALL(videoSender, initFilterCtx()).Times(0);

    EXPECT_EQ(videoSender.begin(), 1);

    EXPECT_NE(videoSender.getDecoderCtx(), nullptr);
    EXPECT_EQ(videoSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(videoSender.getOutputStream(), nullptr);
    EXPECT_EQ(videoSender.getFilterCtx().filter_graph, nullptr);

    videoSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(videoSender.isRunning());
    EXPECT_EQ(videoSender.end(), 1);
    ASSERT_FALSE(videoSender.isRunning());
}

// Tests VideoSender encoded stream mode
TEST_F(VideoSenderTest, RunEncodedVideoStreamMode) {  
    Config::setInt(STREAM_MODE, STREAM_MODE_ENCODED);

    EXPECT_CALL(videoSender, initDecoderCtx()).Times(1);
    EXPECT_CALL(videoSender, initEncoderCtx()).Times(1);
    EXPECT_CALL(videoSender, initFilterCtx()).Times(1);

    EXPECT_EQ(videoSender.begin(), 1);

    EXPECT_NE(videoSender.getDecoderCtx(), nullptr);
    EXPECT_NE(videoSender.getEncoderCtx(), nullptr);
    EXPECT_NE(videoSender.getFilterCtx().filter_graph, nullptr);
    EXPECT_EQ(videoSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(videoSender.getOutputStream(), nullptr);

    videoSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(videoSender.isRunning());
    EXPECT_EQ(videoSender.end(), 1);
    ASSERT_FALSE(videoSender.isRunning());
}

// Tests AudioSender raw stream mode
TEST_F(AudioSenderTest, RunRawAudioStreamMode) {   
    EXPECT_CALL(audioSender, initDecoderCtx()).Times(1);
    EXPECT_CALL(audioSender, initEncoderCtx()).Times(0);
    EXPECT_CALL(audioSender, initFilterCtx()).Times(0);

    EXPECT_EQ(audioSender.begin(), 1);

    EXPECT_NE(audioSender.getDecoderCtx(), nullptr);
    EXPECT_EQ(audioSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(audioSender.getOutputStream(), nullptr);
    EXPECT_EQ(audioSender.getFilterCtx().filter_graph, nullptr);

    audioSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(audioSender.isRunning());
    EXPECT_EQ(audioSender.end(), 1);
    ASSERT_FALSE(audioSender.isRunning());
}

// Tests AudioSender encoded stream mode
TEST_F(AudioSenderTest, RunEncodedAudioStreamMode) {  
    Config::setInt(STREAM_MODE, STREAM_MODE_ENCODED);

    EXPECT_CALL(audioSender, initDecoderCtx()).Times(1);
    EXPECT_CALL(audioSender, initEncoderCtx()).Times(1);
    EXPECT_CALL(audioSender, initFilterCtx()).Times(1);

    EXPECT_EQ(audioSender.begin(), 1);

    EXPECT_NE(audioSender.getDecoderCtx(), nullptr);
    EXPECT_NE(audioSender.getEncoderCtx(), nullptr);
    EXPECT_NE(audioSender.getFilterCtx().filter_graph, nullptr);
    EXPECT_EQ(audioSender.getOutputFormatCtx(), nullptr);
    EXPECT_EQ(audioSender.getOutputStream(), nullptr);

    audioSender.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(audioSender.isRunning());
    EXPECT_EQ(audioSender.end(), 1);
    ASSERT_FALSE(audioSender.isRunning());
}