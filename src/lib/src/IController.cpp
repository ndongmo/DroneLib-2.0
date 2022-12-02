#include "IController.h"

void IController::handleError(int error) {
	if(error > m_error) {
		m_mutex.lock();
		{
			m_error = error;
			m_state = utils::APP_ERROR;
		}
		m_mutex.unlock();
		m_cv.notify_all();
	}
}

void IController::updateState(utils::AppState state) {
	if(state != m_state) {
		m_mutex.lock();
		{
			m_state = state;
			m_error = 0;
		}
		m_mutex.unlock();
		m_cv.notify_all();
	}
}