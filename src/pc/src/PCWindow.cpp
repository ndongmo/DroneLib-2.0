#include "PCWindow.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

#define TEXT_MARGIN 5

int PCWindow::begin() {	
	m_width = Config::getInt(VIDEO_DST_WIDTH);
    m_height = Config::getInt(VIDEO_DST_HEIGHT);
    m_format = Config::getInt(VIDEO_FORMAT);
    
    if(SDL_WasInit(SDL_INIT_EVERYTHING) != SDL_INIT_EVERYTHING) {
        if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            logE << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
            return -1;
        }
    }
	if(SDL_CreateWindowAndRenderer(m_width, m_height, 0, &m_window, &m_renderer) < 0) {
		logE << "Error creating window or renderer: " << SDL_GetError() << std::endl;
		return -1;
	}
    if((m_basic_texture = SDL_CreateTexture(m_renderer, getPixelFormat(), 
		SDL_TEXTUREACCESS_STREAMING, m_width, m_height)) == nullptr) {
        logE << "Error creating streaming texture: " << SDL_GetError() << std::endl;
		return -1;
    }
    if(TTF_WasInit() < 1) {
        if(TTF_Init() == -1) {
            logE << "Error initializing ttf font: " << TTF_GetError() << std::endl;
            return -1;
        }
    }
    if((m_font = TTF_OpenFont(PC_FONT_TYPE_DEFAULT, Config::getInt(PC_FONT_SIZE))) == nullptr) {
        logE << "Font initialization error: " << TTF_GetError() << std::endl;
		return -1;
    }

    SDL_SetWindowTitle(m_window, Config::getString(PC_APP_NAME).c_str());
    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
    m_txt_color = { 0x00, 0x00, 0x00, 0xFF };
    m_back_color = { 0xFF, 0xFF, 0xFF, 0x80 };

    m_evHandler.init();
	initEvents();

	return 1;
}

int PCWindow::clean() {	
    m_running = false;
    m_evHandler.destroy();

    //always destroy window first!!!
    if(m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
	if(m_renderer != nullptr) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if(m_basic_texture != NULL) {
        SDL_DestroyTexture(m_basic_texture);
        m_basic_texture = NULL;
    }
    if(m_font_texture != nullptr) {
        SDL_DestroyTexture(m_font_texture);
        m_font_texture = nullptr;
    }
    if(m_font != nullptr) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }

	return 1;
}

int PCWindow::end() {	
    if(clean() == -1) {
        return -1;
    }

    TTF_Quit();
	SDL_Quit();

	return 1;
}

void PCWindow::start() {
	m_running = true;
}

void PCWindow::run() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (m_evHandler.getPlayWith() == PlayWith::KEYBOARD)
        {
            switch (ev.type) {
            case SDL_QUIT:
                m_evHandler.pressKey(SDLK_ESCAPE);
                break;
            case SDL_KEYDOWN:
                m_evHandler.pressKey(ev.key.keysym.sym);
                break;
            case SDL_KEYUP:
                m_evHandler.releaseKey(ev.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                m_evHandler.pressKey(ev.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                m_evHandler.releaseKey(ev.button.button);
                break;
            }
        }
        else
        {
            switch (ev.type) {
            case SDL_QUIT:
                m_evHandler.pressKey(Joystick::START);
                break;
            case SDL_JOYBUTTONDOWN:
                m_evHandler.pressKey(ev.jbutton.button + JOYSTICK_OFFSET);
                break;
            case SDL_JOYBUTTONUP:
                m_evHandler.releaseKey(ev.jbutton.button + JOYSTICK_OFFSET);
                break;
            case SDL_JOYAXISMOTION:
                m_evHandler.updateJoystickAxis(ev.jaxis.axis, ev.jaxis.value);
                break;
            case SDL_JOYHATMOTION:
                m_evHandler.updateJoystickHats(ev.jhat.value + JOYSTICK_OFFSET);
                break;
            }
        }
    }
}

void PCWindow::initEvents() {
    if(!m_evHandler.loadConfig()) {
        m_evHandler.addEvent(CtrlEvent::QUIT, SDLK_ESCAPE, Joystick::START);
        m_evHandler.addEvent(CtrlEvent::DISCOVER, SDLK_F1, Joystick::SELECT);
        m_evHandler.addEvent(CtrlEvent::GO_UP, SDLK_w, Joystick::AXE1_UP);
        m_evHandler.addEvent(CtrlEvent::GO_DOWN, SDLK_s, Joystick::AXE1_DOWN);
        m_evHandler.addEvent(CtrlEvent::GO_LEFT, SDLK_a, Joystick::AXE1_LEFT);
        m_evHandler.addEvent(CtrlEvent::GO_RIGHT, SDLK_d, Joystick::AXE1_RIGHT);
        m_evHandler.addEvent(CtrlEvent::GO_SPEED_1, SDLK_o, Joystick::A);
        m_evHandler.addEvent(CtrlEvent::GO_SPEED_2, SDLK_p, Joystick::B);
        m_evHandler.saveConfig();
    }
}

void PCWindow::render(const IStreamListener& stream) {
    if(stream.hasNewFrame()) {
        unsigned char * texture_data = NULL;
	    int texture_pitch = 0;

        SDL_LockTexture(m_basic_texture, 0, (void **)&texture_data, &texture_pitch);
        memcpy(texture_data, stream.getData(), stream.getSize());
        SDL_UnlockTexture(m_basic_texture);
    }

    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_basic_texture, NULL, NULL);
    SDL_RenderCopy(m_renderer, m_font_texture, NULL, &m_font_rect);
    SDL_RenderPresent(m_renderer);
}

void PCWindow::updateState(utils::AppState state, int error) {
    std::string str;

    if(state == APP_ERROR) {
        str = logError(error) + " Press [" + m_evHandler.getMapping(CtrlEvent::DISCOVER)
            + "] to restart discovering.";
    }
    else if(state == APP_DISCOVERING) {
        str = "Drone discovering ongoing on ip [" + 
            Config::getString(DRONE_ADDRESS) + "] and port [" +
            std::to_string(Config::getInt(DRONE_PORT_DISCOVERY)) + "] ...";
    }
    else if(state == APP_RUNNING) {
        str = "FPS: " + std::to_string(Config::getInt(VIDEO_FPS));

        if(Config::getInt(VIDEO_WIDTH) != m_width || 
            Config::getInt(VIDEO_HEIGHT) != m_height || 
            Config::getInt(VIDEO_FORMAT) != m_format) {

            Config::setInt(VIDEO_DST_WIDTH, Config::getInt(VIDEO_WIDTH));
            Config::setInt(VIDEO_DST_HEIGHT, Config::getInt(VIDEO_HEIGHT));

            clean();
            begin();
            start();
        }
    }

    if(m_font_texture != nullptr) {
        SDL_DestroyTexture(m_font_texture);
        m_font_texture = nullptr;
    }

    SDL_Surface* text_surf = TTF_RenderText_Shaded_Wrapped(m_font, str.c_str(), 
        m_txt_color, m_back_color, m_width - (TEXT_MARGIN * 2));
    m_font_texture = SDL_CreateTextureFromSurface(m_renderer, text_surf);
    m_font_rect = {10, 10, text_surf->w, text_surf->h};
    SDL_FreeSurface(text_surf);
}

SDL_PixelFormatEnum PCWindow::getPixelFormat() {
    if(m_format == AV_PIX_FMT_YUV420P) {
        return SDL_PIXELFORMAT_YVYU;
    } else  {
        return SDL_PIXELFORMAT_YUY2;
    }
}