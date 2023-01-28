#include "FPSLimiter.h"

#include <utils/Config.h>
#include <utils/Constants.h>

#include <SDL/SDL.h>

using namespace utils;

void FPSLimiter::begin() {
    m_startTicks = SDL_GetTicks();
}

void FPSLimiter::end() {
    calculateFPS();

    float frameTicks = (float) (SDL_GetTicks() - m_startTicks);
    if (1000.0f / Config::getIntVar(VIDEO_FPS) > frameTicks) {
        SDL_Delay((unsigned int)(1000.0f / Config::getIntVar(VIDEO_FPS) - frameTicks));
    }
}

void FPSLimiter::calculateFPS() {
    static float frameTimes[NUM_SAMPLES];
    static int currentFrame = 0;

    static float prevTiks = (float) SDL_GetTicks();

    float currentTick = (float) SDL_GetTicks();

    m_frameTime = currentTick - prevTiks;
    frameTimes[currentFrame % NUM_SAMPLES] = m_frameTime;

    currentFrame++;
    prevTiks = currentTick;

    int count = (currentFrame < NUM_SAMPLES) ? currentFrame : NUM_SAMPLES;

    float frameTimeAverage = 0;
    for (int i = 0; i < count; i++) {
        frameTimeAverage += frameTimes[i];
    }
    frameTimeAverage /= count;

    if (frameTimeAverage > 0) {
        m_fps = 1000.0f / frameTimeAverage;
    } else {
        m_fps = 60.0f;
    }
}