/*!
* \file ComponentController.h
* \brief Hardware component generic controller class.
* \author Ndongmo Silatsa
* \date 21-09-2022
* \version 2.0
*/

#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <condition_variable>

#include <Service.h>

namespace controller {

/*!
 * Hardware component generic controller class in charge of 
 * handling component actions coming in queue.
 * @param T action type
 * @param C max number of components
 */
template <typename T, unsigned int C>
class ComponentController : Service
{
public:    
    void start() override;
    int end() override;
    bool isRunning() override;

    /**
     * Get the component state.
     * @param componentIndex index of the component
     * \return true when the component is on, false otherwise
     */
    bool isOn(unsigned int componentIndex);

    /**
     * @brief Get the component current value.
     * 
     * @param componentIndex index of the component
     * @return current value of the component 
     */
    unsigned int getValue(unsigned int componentIndex);

    /**
     * Add component action in the queue.
     * @param action action to add
     */
    void addAction(const T& action);

    /**
     * Add component actions in the queue.
     * @param action action to add
     */
    void addActions(const std::vector<T>& actions);

protected:
    void run() override;

    /** Wait for the next action */
    void waitNextAction();

    /** Handle action in the queue or/and in the list of actions */
    virtual void handleActions() = 0;

    /** Mutex for queue synchronization */
    std::mutex m_mutex;
    /** Condition variable for controlling access to the mutex */
    std::condition_variable m_cv;
    /** Component handling thread */
    std::thread m_process;

    /** Next action delay in millisecond */
    int m_delay = 0;
    /** Elapsed time since the last loop */
    std::chrono::steady_clock::time_point m_previousClock;

    /** Queue of component actions*/
    std::queue<T> m_queue;
    /** Current executing component actions */
    std::unordered_map<unsigned int, T> m_actions;
    /** Component value map */
    std::unordered_map<unsigned int, unsigned int> m_components;
};

template<typename T, unsigned int C>
void ComponentController<T, C>::start() {
    m_running = true;
    m_process = std::thread([this]{run(); });
}

template<typename T, unsigned int C>
int ComponentController<T, C>::end() {
    m_mutex.lock();
    {
        m_running = false;
        m_components.clear();
        m_actions.clear();
        while(!m_queue.empty()) m_queue.pop();
    }
    m_mutex.unlock();
    m_cv.notify_all();

    if(m_process.joinable()) {
        m_process.join();
    }

    return 1;
}

template<typename T, unsigned int C>
bool ComponentController<T, C>::isRunning() {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_running;
}

template<typename T, unsigned int C>
void ComponentController<T, C>::run() {
    while (isRunning()) {
        handleActions();
        waitNextAction();
    }
}

template<typename T, unsigned int C>
bool ComponentController<T, C>::isOn(unsigned int index) {
    if(index < C && m_components.find(index) != m_components.end()) {
        return m_components[index] != 0;
    }
    return false;
}

template<typename T, unsigned int C>
unsigned int ComponentController<T, C>::getValue(unsigned int index) {
    if(index < C && m_components.find(index) != m_components.end()) {
        return m_components[index];
    }
    return 0;
}

template<typename T, unsigned int C>
void ComponentController<T, C>::addAction(const T& action) {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push(action);
    m_cv.notify_all();
}

template<typename T, unsigned int C>
void ComponentController<T, C>::addActions(const std::vector<T>& actions) {
    std::lock_guard<std::mutex> guard(m_mutex);
    for(const auto& a : actions) {
        m_queue.push(a);
    }
    m_cv.notify_all();
}

template<typename T, unsigned int C>
void ComponentController<T, C>::waitNextAction() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_delay > 0) {
        m_cv.wait_for(lock, std::chrono::milliseconds(m_delay), [this] { 
            return !m_queue.empty() || !m_running;
        });
    } else {
        m_cv.wait(lock, [this] { return !m_queue.empty() || !m_running; });
    }
}

} // controller