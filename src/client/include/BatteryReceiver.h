/*!
* \file BatteryReceiver.h
* \brief Battery receiver class.
* \author Ndongmo Silatsa
* \date 20/09/2023
* \version 2.0
*/

#pragma once

/**
 * Battery receiver class.
 */
class BatteryReceiver
{
public:
    /**
     * @brief Receive new battery life value in percentage.
     * 
     * @param life battery life value (%)
     */
    void newBatteryLife(int life);

    /**
     * @brief Get the battery life value in percentage.
     * 
     * @return life battery life value (%)
     */
    int getBetteryLife();

protected:
    /** Current battery life percentage */
    int m_life;
};