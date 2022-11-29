#include "PCWindow.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

#define TEXT_MARGIN 5

int PCWindow::begin() {	
	m_width = Config::getInt(CTRL_SCREEN_WIDTH, CTRL_SCREEN_WIDTH_DEFAULT);
    m_height = Config::getInt(CTRL_SCREEN_HEIGHT, CTRL_SCREEN_HEIGHT_DEFAULT);
    
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logE << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		return -1;
	}
	if(SDL_CreateWindowAndRenderer(m_width, m_height, 0, &m_window, &m_renderer) < 0) {
		logE << "Error creating window or renderer: " << SDL_GetError() << std::endl;
		return -1;
	}
    if((m_basic_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_BGR24, 
		SDL_TEXTUREACCESS_STREAMING, m_width, m_height)) == nullptr) {
        logE << "Error creating streaming texture: " << SDL_GetError() << std::endl;
		return -1;
    }

    if(TTF_Init() == -1) {
        logE << "Error initializing ttf font: " << TTF_GetError() << std::endl;
		return -1;
    }
    if((m_font = TTF_OpenFont(PC_FONT_TYPE_DEFAULT, PC_FONT_SIZE_DEFAULT)) == nullptr) {
        logE << "Font initialization error: " << TTF_GetError() << std::endl;
		return -1;
    }

    SDL_SetWindowTitle(m_window, PC_APP_NAME);
    
    m_cmd = 0;
    m_state = PC_INIT;
    m_txt_color = { 0x00, 0x00, 0x00, 0xFF };
    m_back_color = { 0xFF, 0xFF, 0xFF, 0x80 };
	m_fps = Config::getInt(CTRL_FPS, CTRL_FPS_DEFAULT);

    m_evHandler.init();
	initEvents();

	return 1;
}

int PCWindow::end() {	
    m_running = false;
    m_evHandler.destroy();

    if(m_font_texture != nullptr) {
        SDL_DestroyTexture(m_font_texture);
        m_font_texture = nullptr;
    }
    if(m_font != nullptr) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
    if(m_basic_texture != nullptr) {
        SDL_DestroyTexture(m_basic_texture);
        m_basic_texture = nullptr;
    }
	if(m_renderer != nullptr) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if(m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
	
    TTF_Quit();
	SDL_Quit();

	return 1;
}

void PCWindow::start() {
	m_running = true;
}

void PCWindow::run() {
	m_prevTicks = SDL_GetTicks();
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (m_evHandler.getPlayWith() == PlayWith::KEYBOARD)
        {
            switch (ev.type) {
            case SDL_QUIT:
                m_cmd = CMD_CTRL_QUIT;
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
                m_cmd = CMD_CTRL_QUIT;
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

    onEvent();

    float frameTicks = (float)(SDL_GetTicks() - m_prevTicks);

    render(frameTicks);

    if (1000.0f / m_fps > frameTicks) {
        SDL_Delay((unsigned int)(1000.0f / (m_fps - frameTicks)));
    }
}

void PCWindow::onEvent() {
	if (m_evHandler.isEventPressed(CtrlEvent::QUIT)) {
		m_cmd = CMD_CTRL_QUIT;
	}
    else if (m_evHandler.isEventPressed(CtrlEvent::DISCOVER)) {
		m_cmd = CMD_CTRL_DISCOVER;
	}
	else if (m_evHandler.isEventDown(CtrlEvent::GO_UP)) {
		
	}
	else if (m_evHandler.isEventDown(CtrlEvent::GO_DOWN)) {
		
	}
	if (m_evHandler.isEventDown(CtrlEvent::GO_LEFT)) {
		
	}
	else if (m_evHandler.isEventDown(CtrlEvent::GO_RIGHT)) {
		
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
        m_evHandler.saveConfig();
    }
}

void PCWindow::render(int elapsedTime) {
    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_font_texture, NULL, &m_font_rect);
    SDL_RenderPresent(m_renderer);
}

int PCWindow::getCmd() {
    int tmp = m_cmd;
    m_cmd = 0;
    return tmp;
}

void PCWindow::updateState(PCState state, int error) {
    if(state == m_state) return;

    std::string str;
    m_state = state;

    if(m_state == PC_ERROR) {
        str = logError(error) + " Press [" + m_evHandler.getMapping(CtrlEvent::DISCOVER)
            + "] to restart discovering.";
    }
    else if(m_state == PC_DISCOVERING) {
        int serverPort = Config::getInt(DRONE_PORT_DISCOVERY, DRONE_PORT_DISCOVERY_DEFAULT);
        std::string serverAddr = Config::getString(DRONE_ADDRESS, DRONE_IPV4_ADDRESS_DEFAULT);

        str = "Drone discovering ongoing on ip [" + serverAddr + "] and port ["
            + std::to_string(serverPort) + "] ...";
    }
    else {
        str = "FPS: " + std::to_string(m_fps);
    }

    if(m_font_texture != nullptr) {
        SDL_DestroyTexture(m_font_texture);
    }

    SDL_Surface* text_surf = TTF_RenderText_Shaded_Wrapped(m_font, str.c_str(), 
        m_txt_color, m_back_color, m_width - (TEXT_MARGIN * 2));
    m_font_texture = SDL_CreateTextureFromSurface(m_renderer, text_surf);
    m_font_rect = {10, 10, text_surf->w, text_surf->h};
    SDL_FreeSurface(text_surf);
}