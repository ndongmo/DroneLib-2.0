/*!
* \file IController.h
* \brief Controller interface.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "utils/Structs.h"

#include <mutex>
#include <condition_variable>

/**
 * Controller interface in charge of exchanging data with listeners.
 */
class IController
{
public:
    /**
     * Handle the given error.
     * @param error error code
     */
    virtual void handleError(int error);

    /**
     * Get the current state.
     * \return current state
     */
    utils::AppState getState() {
        return m_state;
    }

protected:
    /**
     * Update the current state.
     * @param state new state
     */
    void updateState(utils::AppState state);
    
    /** Current state */
    utils::AppState m_state;
    /** Previous app state */
    utils::AppState m_oldState;

    /* Mutex for variables synchronization */
    std::mutex m_mutex;
    /** Condition variable for controlling access to the mutex */
    std::condition_variable m_cv;
    /* Current error */
    int m_error;
};