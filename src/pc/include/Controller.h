/*!
* \file Controller.h
* \brief Main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "EventHandler.h"

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
     * Initialize controller events.
     */
    void initEvents();

    /*!
     * Controller events handler method.
     */
	void onEvent();

    /*!
     * Running state.
     */
    bool m_running = false;
    /*!
     * Number of Frame per second.
     */
    unsigned int m_fps;
    /*!
     * Previously registered timestamp.
     */
    unsigned int m_prevTicks;

    SDL_Window* m_window = nullptr;
	SDL_Texture* m_texture = nullptr;
	SDL_Renderer* m_renderer = nullptr;

    EventHandler m_evHandler;
};