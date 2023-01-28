/*!
* \file FPSLimiter.h
* \brief Calculate and limite game FPS.
* \author Ndongmo Silatsa Fabrice
* \date 27-10-2016
* \version 1.0
*/

#pragma once

#define NUM_SAMPLES 10

/**
 * @brief Class for computing the average fps and limiting
 * the current process work by sleeping the remaining time.
 */
class FPSLimiter {
public:
    /*!
    * \brief Retrieve SDL ticks.
    */
    void begin();
    /*!
    * \brief Calculate and return FPS value.
    */
    void end();

    /* GETTERS */
    unsigned int getFPS() const { return (unsigned int)m_fps; }

private:
    /*!
    * \brief Calculate FPS value.
    */
    void calculateFPS();

    float m_fps = 0;			/* FPS value */
    float m_frameTime;			/* Frame time */
    unsigned int m_startTicks;	/* Keep ticks last value */
};