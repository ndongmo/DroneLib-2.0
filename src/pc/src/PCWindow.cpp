#include "PCWindow.h"
#include "Constants.h"

#include <utils/Logger.h>
#include <utils/Config.h>
#include <utils/Constants.h>

using namespace utils;

#define TEXT_MARGIN 5
#define TEXT_FPS_WIDTH 50
#define TEXT_BATTERIE_WIDTH 50
#define TEXT_LINE_HEIGHT 14

PCWindow::PCWindow() { 
    m_name = "PCWindow";
}

int PCWindow::begin() {	
	m_width = Config::getInt(VIDEO_WIDTH);
    m_height = Config::getInt(VIDEO_HEIGHT);
    m_format = Config::getString(VIDEO_PIX_FORMAT);
    
    if(!SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS)) {
        if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS)) {
            logE << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
            return -1;
        }
    }
	if(!SDL_CreateWindowAndRenderer(m_name.c_str(), m_width, m_height, 0, &m_window, &m_renderer)) {
		logE << "Error creating window or renderer: " << SDL_GetError() << std::endl;
		return -1;
	}
    if((m_basic_texture = SDL_CreateTexture(m_renderer, getPixelFormat(), 
		SDL_TEXTUREACCESS_STREAMING, m_width, m_height)) == nullptr) {
        logE << "Error creating streaming texture: " << SDL_GetError() << std::endl;
		return -1;
    }
    if(!TTF_WasInit()) {
        if(!TTF_Init()) {
            logE << "Error initializing ttf font: " << SDL_GetError() << std::endl;
            return -1;
        }
    }
    if((m_font = TTF_OpenFont(PC_FONT_TYPE_DEFAULT, Config::getInt(PC_FONT_SIZE))) == nullptr) {
        logE << "Font initialization error: " << SDL_GetError() << std::endl;
		return -1;
    }

    SDL_SetWindowTitle(m_window, Config::getString(PC_APP_NAME).c_str());
    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
    m_txt_color = { 0x00, 0x00, 0x00, 0xFF };
    m_back_color = { 0xFF, 0xFF, 0xFF, 0x80 };

     m_fps_text.dynamic = true;
    m_fps_text.rect = { (float)TEXT_MARGIN, (float)TEXT_MARGIN, (float)TEXT_FPS_WIDTH, (float)TEXT_LINE_HEIGHT };

    m_bat_text.dynamic = true;
    m_bat_text.rect = { (float)(m_width - (TEXT_BATTERIE_WIDTH + TEXT_MARGIN)), 
        (float)TEXT_MARGIN, (float)TEXT_BATTERIE_WIDTH, (float)TEXT_LINE_HEIGHT };

    m_msg_text.rect = { (float)TEXT_MARGIN, (float)(m_height - (TEXT_LINE_HEIGHT + TEXT_MARGIN)), 
        (float)(m_width - (TEXT_MARGIN * 2)), (float)TEXT_LINE_HEIGHT };

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
    if(m_basic_texture != nullptr) {
        SDL_DestroyTexture(m_basic_texture);
        m_basic_texture = nullptr;
    }
    if(m_fps_text.texture != nullptr) {
        SDL_DestroyTexture(m_fps_text.texture);
        m_fps_text.texture = nullptr;
    }
    if(m_bat_text.texture != nullptr) {
        SDL_DestroyTexture(m_bat_text.texture);
        m_bat_text.texture = nullptr;
    }
    if(m_msg_text.texture != nullptr) {
        SDL_DestroyTexture(m_msg_text.texture);
        m_msg_text.texture = nullptr;
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

    if(TTF_WasInit()) {
        TTF_Quit();
    }
    if(SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS)) {
        SDL_Quit();
    }

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
            case SDL_EVENT_QUIT:
                m_evHandler.pressKey(SDLK_ESCAPE);
                break;
            case SDL_EVENT_KEY_DOWN:
                m_evHandler.pressKey(ev.key.key);
                break;
            case SDL_EVENT_KEY_UP:
                m_evHandler.releaseKey(ev.key.key);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                m_evHandler.pressKey(ev.button.button);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                m_evHandler.releaseKey(ev.button.button);
                break;
            }
        }
        else
        {
            switch (ev.type) {
            case SDL_EVENT_QUIT:
                m_evHandler.pressKey(Joystick::START);
                break;
            case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
                m_evHandler.pressKey(ev.jbutton.button + JOYSTICK_OFFSET);
                break;
            case SDL_EVENT_JOYSTICK_BUTTON_UP:
                m_evHandler.releaseKey(ev.jbutton.button + JOYSTICK_OFFSET);
                break;
            case SDL_EVENT_JOYSTICK_AXIS_MOTION:
                m_evHandler.updateJoystickAxis(ev.jaxis.axis, ev.jaxis.value);
                break;
            case SDL_EVENT_JOYSTICK_HAT_MOTION:
                m_evHandler.updateJoystickHats(ev.jhat.value + JOYSTICK_OFFSET);
                break;
            }
        }
    }
    m_evHandler.update();
}

bool PCWindow::isActive() const {
    return Config::getInt(CAMERA_ACTIVE);
}

void PCWindow::initEvents() {
    if(!m_evHandler.loadConfig()) {
        m_evHandler.addEvent(ClientEvent::QUIT, SDLK_ESCAPE, Joystick::START);
        m_evHandler.addEvent(ClientEvent::DISCOVER, SDLK_F1, Joystick::SELECT);
        m_evHandler.addEvent(ClientEvent::BUZZ, SDLK_B, Joystick::X);
        m_evHandler.addEvent(ClientEvent::GO_UP, SDLK_W, Joystick::AXE1_UP);
        m_evHandler.addEvent(ClientEvent::GO_DOWN, SDLK_S, Joystick::AXE1_DOWN);
        m_evHandler.addEvent(ClientEvent::GO_LEFT, SDLK_A, Joystick::AXE1_LEFT);
        m_evHandler.addEvent(ClientEvent::GO_RIGHT, SDLK_D, Joystick::AXE1_RIGHT);
        m_evHandler.addEvent(ClientEvent::GO_SPEED_1, SDLK_O, Joystick::A);
        m_evHandler.addEvent(ClientEvent::GO_SPEED_2, SDLK_P, Joystick::B);
        m_evHandler.addEvent(ClientEvent::CAM_UP, SDLK_UP, Joystick::AXE2_UP);
        m_evHandler.addEvent(ClientEvent::CAM_DOWN, SDLK_DOWN, Joystick::AXE2_DOWN);
        m_evHandler.addEvent(ClientEvent::CAM_LEFT, SDLK_LEFT, Joystick::AXE2_LEFT);
        m_evHandler.addEvent(ClientEvent::CAM_RIGHT, SDLK_RIGHT, Joystick::AXE2_RIGHT);
        m_evHandler.saveConfig();
    }
}

void PCWindow::render(const IStreamListener& stream, int life) {
    m_fps_text.text = "FPS: " + std::to_string(Config::getInt(VIDEO_FPS));
    m_bat_text.text = "Bat: " + std::to_string(life) + "%";

    if(stream.hasNewFrame()) {
        unsigned char * texture_data = NULL;
	    int texture_pitch = 0;

        if (SDL_LockTexture(m_basic_texture, 0, (void **)&texture_data, &texture_pitch)) {
            memcpy(texture_data, stream.getData(), stream.getSize());
            SDL_UnlockTexture(m_basic_texture);
        }
    }

    SDL_RenderClear(m_renderer);
    SDL_RenderTexture(m_renderer, m_basic_texture, NULL, NULL);

    renderText(m_fps_text);
    renderText(m_bat_text);
    renderText(m_msg_text);

    SDL_RenderPresent(m_renderer);
}

void PCWindow::updateState(utils::AppState state, int error) {
    if(state == APP_ERROR) {
        m_msg_text.text = logError(error) + " Press [" + 
            m_evHandler.getMapping(ClientEvent::DISCOVER) + "] to restart discovering.";
        updateText(m_msg_text);
    }
    else if(state == APP_DISCOVERING) {
        m_msg_text.text = "Drone discovering ongoing on ip [" + 
            Config::getString(DRONE_ADDRESS) + "] and port [" +
            std::to_string(Config::getInt(DRONE_PORT_DISCOVERY)) + "] ...";
        updateText(m_msg_text);
    }
    else if(state == APP_RUNNING) {
        m_msg_text.text = "";

        if(Config::getInt(CAMERA_ACTIVE) && (Config::getInt(VIDEO_WIDTH) != m_width || 
            Config::getInt(VIDEO_HEIGHT) != m_height || Config::getString(VIDEO_PIX_FORMAT) != m_format)) {
            clean();
            begin();
            start();
        }
    }
}

void PCWindow::renderText(PCText& text) {
    if(!text.text.empty()) {
        if(text.dynamic) {
            updateText(text);
        } 
        SDL_RenderTexture(m_renderer, text.texture, NULL, &text.rect);
    }
}

void PCWindow::updateText(PCText& text) {
    if(text.texture != nullptr) {
        SDL_DestroyTexture(text.texture);
        text.texture = nullptr;
    }

    SDL_Surface* text_surf = TTF_RenderText_Shaded(m_font, text.text.c_str(), 0, 
        m_txt_color, m_back_color);
    if (text_surf != nullptr) {
        text.texture = SDL_CreateTextureFromSurface(m_renderer, text_surf);
        text.rect.w = text_surf->w;
        text.rect.h = text_surf->h;
        SDL_DestroySurface(text_surf);
    }
}

SDL_PixelFormat PCWindow::getPixelFormat() {
    AVPixelFormat format = av_get_pix_fmt(m_format.c_str());
    
    if(format == AV_PIX_FMT_YUV420P) {
        return SDL_PIXELFORMAT_YV12;
    } else  {
        return SDL_PIXELFORMAT_YUY2;
    }
}