#include "ErrorListener.h"

void ErrorListener::add(int error) {
    std::lock_guard<std::mutex> guard(m_error_mutex);
    m_error_queue.emplace(error);
}

void ErrorListener::handleErrors() {
    m_error_mutex.lock();
    while(!m_error_queue.empty()) {
        int error = m_error_queue.front();

        m_error_queue.pop();
        if(m_error_occurences.find(error) != m_error_occurences.end()) {
            m_error_occurences[error]++;
        } else {
            m_error_occurences[error] = 1;
        }

        handleError(error);
    }
    m_error_mutex.unlock();
}