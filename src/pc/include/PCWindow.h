/*!
* \file PCWindow.h
* \brief PC window ui class.
* \author Ndongmo Silatsa
* \date 25-14-2022
* \version 2.0
*/

#pragma once

#include "EventHandler.h"

#include <utils/Structs.h>
#include <stream/Stream.h>

#include <Service.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

using namespace stream;

/**
 * PC window ui class which displays the video output,
 * error messages and handles keyboard/mouse/joystick events.
 */
class PCWindow : public Service
{
public:
    /**
     * Constructor for initializing the event handler
     */
    PCWindow(EventHandler& evHandler);

    int begin() override;
    void start() override;
    int end() override;
    void run() override;

    /**
     * Render the current frame.
     * @param stream stream frame provider
     */
    void render(const IStreamListener& stream);

    /**
     * Update the current state.
     * @param state the new state
     * @param error current error
     */
    void updateState(utils::AppState state, int error);

private:
    /**
     * Initialize controller events.
     */
    void initEvents();

    /**
     * Clean the SDL object instances.
     */
    int clean();

    /**
     * @brief Get the Pixel Format value for the current video format.
     * 
     * @return an SDL_PixelFormatEnum 
     */
    SDL_PixelFormatEnum getPixelFormat();

    /* Window width */
    int m_width;
    /* Window height */
    int m_height;
    /** Stream video format */
    int m_format;

    SDL_Color m_txt_color;
    SDL_Color m_back_color;
    SDL_Rect m_font_rect;

    TTF_Font * m_font = nullptr;
    SDL_Window* m_window = nullptr;
	SDL_Texture* m_basic_texture = nullptr;
    SDL_Texture* m_font_texture = nullptr;
	SDL_Renderer* m_renderer = nullptr;

    EventHandler& m_evHandler;
};