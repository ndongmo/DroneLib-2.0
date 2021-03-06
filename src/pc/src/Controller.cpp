#include "Controller.h"
#include "Types.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

#define DEFAULT_FPS 60
#define DEFAULT_SCREEN_WIDTH 640
#define DEFAULT_SCREEN_HEIGHT 480

using namespace utils;

int Controller::begin()
{	
    int err = 1;
	int width = Config::getInt(CTRL_SCREEN_WIDTH, DEFAULT_SCREEN_WIDTH);
    int height = Config::getInt(CTRL_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT);

	if (width < 1 || height < 1) {
        logE << "Bad window width[" << width << "] and height["<< height 
            << "] configuration" << std::endl;
        return -1;
    }

    m_fps = Config::getInt(CTRL_FPS, DEFAULT_FPS);
    if(m_fps < 1) {
        m_fps = DEFAULT_FPS;
    }

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logE << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		return -1;
	}
	if (SDL_CreateWindowAndRenderer(width, height, 0, &m_window, &m_renderer) < 0)
	{
		logE << "Error creating window or renderer : " << SDL_GetError() << std::endl;
		return -1;
	}
	m_texture = SDL_CreateTexture(m_renderer, 
		SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, width, height);

    m_evHandler.init();
	initEvents();

	return err;
}

int Controller::end()
{
	int err = 1;
	
    m_evHandler.destroy();

	SDL_DestroyTexture(m_texture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	return err;
}

void Controller::start()
{
	run();
}

void Controller::run()
{
	m_running = true;

	while (m_running)
	{
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
	end();
}

void Controller::onEvent()
{
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

void Controller::initEvents()
{
    if(!m_evHandler.loadConfig()) {
        m_evHandler.addEvent(CtrlEvent::QUIT, SDLK_ESCAPE, Joystick::START);
        m_evHandler.addEvent(CtrlEvent::GO_UP, SDLK_w, Joystick::AXE1_UP);
        m_evHandler.addEvent(CtrlEvent::GO_DOWN, SDLK_s, Joystick::AXE1_DOWN);
        m_evHandler.addEvent(CtrlEvent::GO_LEFT, SDLK_a, Joystick::AXE1_LEFT);
        m_evHandler.addEvent(CtrlEvent::GO_RIGHT, SDLK_d, Joystick::AXE1_RIGHT);
        m_evHandler.saveConfig();
    }
}