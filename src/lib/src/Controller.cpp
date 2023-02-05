#include "Controller.h"
#include "utils/Config.h"
#include "utils/Constants.h"

using namespace utils;

void Controller::initConfigs() {
    Config::setIntDefault(VIDEO_FPS, VIDEO_FPS_DEFAULT);
	Config::setIntDefault(VIDEO_CODEC, VIDEO_CODEC_DEFAULT);
	Config::setIntDefault(VIDEO_FORMAT, VIDEO_FORMAT_DEFAULT);
	Config::setIntDefault(VIDEO_WIDTH, VIDEO_WIDTH_DEFAULT);
	Config::setIntDefault(VIDEO_HEIGHT, VIDEO_HEIGHT_DEFAULT);
    Config::setStringDefault(VIDEO_DEVICE, VIDEO_DEVICE_DEFAULT);

	Config::setIntDefault(AUDIO_CHANNELS, AUDIO_CHANNELS_DEFAULT);
	Config::setIntDefault(AUDIO_BIT_RATE, AUDIO_BIT_RATE_DEFAULT);
	Config::setIntDefault(AUDIO_SAMPLE, AUDIO_SAMPLE_DEFAULT);
	Config::setIntDefault(AUDIO_CODEC, AUDIO_CODEC_DEFAULT);
	Config::setIntDefault(AUDIO_FORMAT, AUDIO_FORMAT_DEFAULT);
    Config::setStringDefault(AUDIO_DEVICE, AUDIO_DEVICE_DEFAULT);
    Config::setStringDefault(AUDIO_INPUT_FORMAT, AUDIO_INPUT_FORMAT_DEFAULT);

	Config::setIntDefault(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
	Config::setStringDefault(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);
}