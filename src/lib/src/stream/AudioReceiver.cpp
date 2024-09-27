#include "stream/AudioReceiver.h"
#include "utils/Config.h"
#include "utils/Constants.h"

namespace stream {

AudioReceiver::AudioReceiver() {
    m_name = "AudioReceiverService";
}

bool AudioReceiver::isActive() const {
    return utils::Config::getInt(MICRO_ACTIVE);
}
}