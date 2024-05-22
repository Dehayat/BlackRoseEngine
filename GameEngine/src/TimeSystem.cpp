#include "TimeSystem.h"
#include "Logger.h"
#include <cstdint>
#include <SDL2/SDL.h>

TimeSystem::TimeSystem()
{
	dt = 0;
	msLastFrame = 0;
	capFrameRate = false;
}

void TimeSystem::Update()
{
	if (capFrameRate && SDL_GetTicks64() - msLastFrame < FRAMETIME_MS) {
		std::uint64_t waitTimeMs = FRAMETIME_MS - (SDL_GetTicks64() - msLastFrame);
		SDL_Delay(waitTimeMs);
	}
	dt = (SDL_GetTicks64() - msLastFrame) / 1000.0f;
	msLastFrame = SDL_GetTicks64();
}

float TimeSystem::GetdeltaTime()
{
	return dt;
}
