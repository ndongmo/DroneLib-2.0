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
 * @brief Shown text structure.
 */
struct PCText {
    /** Dynamic text */
    bool dynamic = false;
    /** Text to show */
    std::string text;
    /** Texture of the text */
    SDL_Texture *texture = nullptr;
    /** Texture rect */
    SDL_Rect rect;
};

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
    PCWindow();

    int begin() override;
    void start() override;
    int end() override;
    void run() override;

    /**
     * Render the current frame.
     * @param stream stream frame provider
     * @param life battery life
     */
    void render(const IStreamListener& stream, int life);

    /**
     * Update the current state.
     * @param state the new state
     * @param error current error
     */
    void updateState(utils::AppState state, int error);

    /**
     * @brief Get a reference of the event handler.
     * 
     * @return a reference of the event handler
     */
    EventHandler& getEventHandler() { return m_evHandler; }

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
     * @brief Render the given text structure.
     * 
     * @param text the text structure to render
     */
    void renderText(PCText& text);

    /**
     * @brief Print the given text into the texture.
     * 
     * @param text text structure
     */
    void updateText(PCText& text);

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

    /** FPS text */
    PCText m_fps_text;
    /** Batterie life text */
    PCText m_bat_text;
    /** Message text */
    PCText m_msg_text;

    SDL_Color m_txt_color;
    SDL_Color m_back_color;

    TTF_Font * m_font = nullptr;
    SDL_Window* m_window = nullptr;
	SDL_Texture* m_basic_texture = nullptr;
	SDL_Renderer* m_renderer = nullptr;

    EventHandler m_evHandler;
};