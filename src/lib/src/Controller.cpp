#include "Controller.h"
#include "utils/Config.h"
#include "utils/Logger.h"
#include "utils/Constants.h"

using namespace utils;

void Controller::initConfigs() {
	Config::setIntDefault(CAMERA_ACTIVE, CAMERA_ACTIVE_DEFAULT);
	Config::setIntDefault(MICRO_ACTIVE, MICRO_ACTIVE_DEFAULT);
	Config::setIntDefault(SERVOS_ACTIVE, SERVOS_ACTIVE_DEFAULT);
	Config::setIntDefault(MOTORS_ACTIVE, MOTORS_ACTIVE_DEFAULT);
	Config::setIntDefault(LEDS_ACTIVE, LEDS_ACTIVE_DEFAULT);
	Config::setIntDefault(BUZZER_ACTIVE, BUZZER_ACTIVE_DEFAULT);
	Config::setIntDefault(BATTERY_ACTIVE, BATTERY_ACTIVE_DEFAULT);

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
	Config::setStringDefault(AUDIO_ENCODER, AUDIO_ENCODER_DEFAULT);
    Config::setStringDefault(AUDIO_DEVICE, AUDIO_DEVICE_DEFAULT);
    Config::setStringDefault(AUDIO_INPUT_FORMAT, AUDIO_INPUT_FORMAT_DEFAULT);

	Config::setIntDefault(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
	Config::setStringDefault(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

	Config::setIntDefault(STREAM_MODE, STREAM_MODE_DEFAULT);
	Config::setStringDefault(STREAM_PROTOCOL, STREAM_PROTOCOL_DEFAULT);
	Config::setStringDefault(STREAM_OUTPUT_FORMAT, STREAM_OUTPUT_FORMAT_DEFAULT);
	Config::setStringDefault(STREAM_OUT_FILE_ADDRESS, STREAM_OUT_FILE_ADDRESS_DEFAULT);
}

int Controller::beginService(Service& service) {
	if(service.isActive()) {
		if(service.begin() == -1) {
			logE << getName() << ": " << service.getName() << " was initialized unsuccessfully!" << std::endl;
			return -1;
		} else {
			logI << getName() << ": " << service.getName() << " was initialized!" << std::endl;
		}
	} else {
		logW << getName() << ": " << service.getName() << " is deactivated!" << std::endl;
	}
	return 1;
}

void Controller::startService(Service& service) {
	if(service.isActive() && !service.isRunning()) {
		service.start();
		logI << getName() << ": " << service.getName() << " was started!" << std::endl;
	}
}

void Controller::stopService(Service& service) {
	if(service.isActive() && service.isRunning()) {
		service.stop();
		logI << getName() << ": " << service.getName() << " was stopped!" << std::endl;
	}
}

int Controller::endService(Service& service) {
	if(service.isActive()) {
		if(service.end() == -1) {
			logE << getName() << ": " << service.getName() << " was closed unsuccessfully!" << std::endl;
			return -1;
		} else {
			logI << getName() << ": " << service.getName() << " was closed!" << std::endl;
		}
	} 
	return 1;
}