#include "PCWindow.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

int PCWindow::begin() {	
	int width = Config::getInt(CTRL_SCREEN_WIDTH, CTRL_SCREEN_WIDTH_DEFAULT);
    int height = Config::getInt(CTRL_SCREEN_HEIGHT, CTRL_SCREEN_HEIGHT_DEFAULT);
    
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logE << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		return -1;
	}
	if (SDL_CreateWindowAndRenderer(width, height, 0, &m_window, &m_renderer) < 0) {
		logE << "Error creating window or renderer : " << SDL_GetError() << std::endl;
		return -1;
	}

	m_fps = Config::getInt(CTRL_FPS, CTRL_FPS_DEFAULT);
	m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_BGR24, 
		SDL_TEXTUREACCESS_STREAMING, width, height);

    m_evHandler.init();
	initEvents();

    if(m_window == nullptr || m_renderer == nullptr || m_texture == nullptr) {
        return -1;
    }

	return 1;
}

int PCWindow::end() {	
    m_running = false;
    m_evHandler.destroy();
	SDL_DestroyTexture(m_texture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
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
                m_running = false;
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
                m_running = false;
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

    if (1000.0f / m_fps > frameTicks) {
        SDL_Delay((unsigned int)(1000.0f / (m_fps - frameTicks)));
    }
}

void PCWindow::onEvent() {
	if (m_evHandler.isEventPressed(CtrlEvent::QUIT)) {
		m_running = false;
        return;
	}
	if (m_evHandler.isEventDown(CtrlEvent::GO_UP)) {
		
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
        m_evHandler.addEvent(CtrlEvent::GO_UP, SDLK_w, Joystick::AXE1_UP);
        m_evHandler.addEvent(CtrlEvent::GO_DOWN, SDLK_s, Joystick::AXE1_DOWN);
        m_evHandler.addEvent(CtrlEvent::GO_LEFT, SDLK_a, Joystick::AXE1_LEFT);
        m_evHandler.addEvent(CtrlEvent::GO_RIGHT, SDLK_d, Joystick::AXE1_RIGHT);
        m_evHandler.saveConfig();
    }
}