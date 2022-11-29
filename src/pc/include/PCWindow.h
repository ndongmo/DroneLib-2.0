/*!
* \file PCController.h
* \brief PC main controller class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "EventHandler.h"
#include "PCHelper.h"

#include <Service.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

/*!
 * PC controller service class which starts and stops
 * all others services and handle the input events.
 */
class PCWindow : public Service
{
public:
    int begin() override;
    void start() override;
    int end() override;
    void run() override;

    /*!
     * Get the Event Handler object
     * 
     * @return const EventHandler& 
     */
    const EventHandler& getEventHandler() const {
        return m_evHandler;
    }

    /*!
     * Obtain the current command and reset it.
     * @return current command ID
     */
    int getCmd();

    /*!
     * Update the current state.
     * @param state the new state
     * @param error current error
     */
    void updateState(PCState state, int error);

private:

    /*!
     * Initialize controller events.
     */
    void initEvents();

    /*!
     * Controller events handler method.
     */
	void onEvent();

    /*!
     * Render the current frame.
     * @param elapsedTime time since the last call
     */
    void render(int elapsedTime);

    /* Number of Frame per second */
    unsigned int m_fps;
    /* Previously registered timestamp */
    unsigned int m_prevTicks;
    /* Current command */
    unsigned int m_cmd;
    /* Window width */
    unsigned int m_width;
    /* Window height */
    unsigned int m_height;
    /* Current state */
    PCState m_state;

    SDL_Color m_txt_color;
    SDL_Color m_back_color;
    SDL_Rect m_font_rect;

    TTF_Font * m_font = nullptr;
    SDL_Window* m_window = nullptr;
	SDL_Texture* m_basic_texture = nullptr;
    SDL_Texture* m_font_texture = nullptr;
	SDL_Renderer* m_renderer = nullptr;

    EventHandler m_evHandler;
};