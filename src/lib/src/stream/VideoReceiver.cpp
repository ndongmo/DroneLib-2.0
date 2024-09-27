#include "stream/VideoReceiver.h"
#include "utils/Config.h"
#include "utils/Constants.h"

namespace stream {

VideoReceiver::VideoReceiver() {
	m_name = "VideoReceiverService";
}

bool VideoReceiver::isActive() const {
    return utils::Config::getInt(CAMERA_ACTIVE);
}

} // namespace stream