/*!
* \file Controller.h
* \brief Main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include <Service.h>
#include <SDL/SDL.h>

/*!
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class Controller : public Service
{
public:
    int begin() override;
    void start() override;
    int end() override;

private:
    void run() override;

    /*!
     * Running state.
     */
    bool m_running = false;
};