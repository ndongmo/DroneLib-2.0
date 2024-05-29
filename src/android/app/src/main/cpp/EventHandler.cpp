#include "EventHandler.h"

void EventHandler::init() {
	for(size_t i = 0; i < EVENT_COUNT; i++) {
		m_eventMap[i] = 0;
	}
}

void EventHandler::pressEvent(unsigned int eventID) {
	m_eventMap[eventID] = true;
}

void EventHandler::releaseEvent(unsigned int eventID) {
	m_eventMap[eventID] = false;
}

bool EventHandler::isEventDown(unsigned int eventID) {
	auto it = m_eventMap.find(eventID);
	if (it != m_eventMap.end()) {
		return it->second;
	}

	return false;
}

bool EventHandler::isEventPressed(unsigned int eventID) {
	bool result = false;
	auto it = m_eventMap.find(eventID);

	if (it != m_eventMap.end()) {
		result = it->second;
		it->second = false; // set this key to false
	}

	return result;
}

std::string EventHandler::getMapping(unsigned int eventID)
{
	switch (eventID)
		{
		case ClientEvent::GO_SPEED_2: return "A button";
		case ClientEvent::BUZZ: return "B button";
		case ClientEvent::QUIT: return "Quit button";
		case ClientEvent::DISCOVER: return "Discover button";
		case ClientEvent::GO_UP: return "Move UP";
		case ClientEvent::GO_DOWN: return "Move DOWN";
		case ClientEvent::GO_LEFT: return "Move LEFT";
		case ClientEvent::GO_RIGHT: return "Move RIGHT";
		case ClientEvent::CAM_UP: return "Camera look UP";
		case ClientEvent::CAM_DOWN: return "Camera look DOWN";
		case ClientEvent::CAM_LEFT: return "Camera look LEFT";
		case ClientEvent::CAM_RIGHT: return "Camera RIGHT";
		default:
			return "";
		}
}