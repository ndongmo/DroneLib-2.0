#include "Service.h"

void Service::sendError(int error) {
    if(m_error_listener != nullptr) {
        m_error_listener->add(error);
    }
}