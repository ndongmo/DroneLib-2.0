#include "Service.h"
#include "IController.h"

void Service::sendError(int error) {
    if(m_controller != nullptr) {
        m_controller->handleError(error);
    }
}