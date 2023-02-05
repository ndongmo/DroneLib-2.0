#include "Controller.h"
#include "utils/Config.h"
#include "utils/Constants.h"

using namespace utils;

void Controller::initConfigs() {
    Config::setIntVar(VIDEO_FPS, Config::getInt(VIDEO_FPS, VIDEO_FPS_DEFAULT));
	Config::setIntVar(VIDEO_CODEC, Config::getInt(VIDEO_CODEC, VIDEO_CODEC_DEFAULT));
	Config::setIntVar(VIDEO_FORMAT, Config::getInt(VIDEO_FORMAT, VIDEO_FORMAT_DEFAULT));
	Config::setIntVar(VIDEO_WIDTH, Config::getInt(VIDEO_WIDTH, VIDEO_WIDTH_DEFAULT));
	Config::setIntVar(VIDEO_HEIGHT, Config::getInt(VIDEO_HEIGHT, VIDEO_HEIGHT_DEFAULT));
    Config::setStringVar(VIDEO_DEVICE, Config::getString(VIDEO_DEVICE, VIDEO_DEVICE_DEFAULT));

	Config::setIntVar(AUDIO_CHANNELS, Config::getInt(AUDIO_CHANNELS, AUDIO_CHANNELS_DEFAULT));
	Config::setIntVar(AUDIO_BIT_RATE, Config::getInt(AUDIO_BIT_RATE, AUDIO_BIT_RATE_DEFAULT));
	Config::setIntVar(AUDIO_SAMPLE, Config::getInt(AUDIO_SAMPLE, AUDIO_SAMPLE_DEFAULT));
	Config::setIntVar(AUDIO_CODEC, Config::getInt(AUDIO_CODEC, AUDIO_CODEC_DEFAULT));
	Config::setIntVar(AUDIO_FORMAT, Config::getInt(AUDIO_FORMAT, AUDIO_FORMAT_DEFAULT));
    Config::setStringVar(AUDIO_DEVICE, Config::getString(AUDIO_DEVICE, AUDIO_DEVICE_DEFAULT));
    Config::setStringVar(AUDIO_INPUT_FORMAT, Config::getString(AUDIO_INPUT_FORMAT, AUDIO_INPUT_FORMAT_DEFAULT));

	Config::setIntVar(DRONE_PORT_DISCOVERY, Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT));
	Config::setStringVar(DRONE_ADDRESS, Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT));
}