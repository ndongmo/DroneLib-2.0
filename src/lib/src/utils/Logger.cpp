#include "utils/Logger.h"
#include "utils/Constants.h"

utils::Logger utils::Logger::m_logger;

namespace utils {
    Logger::Logger() {
        m_errors[0] = "Unknow error";
        m_errors[ERROR_NET_SEND] = "Error sending data through network.";
        m_errors[ERROR_NET_RECEIVE] = "Error reading received data through network.";
        m_errors[ERROR_NET_DISCOVERY] = "Error discovering network.";
        m_errors[ERROR_NET_INIT] = "Error initializing network.";
    }
}