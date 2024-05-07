#include "TimeSystem.h"
#include <cstdint>
#include <SDL2/SDL.h>

TimeSystem::TimeSystem()
{
	dt = 0;
	msLastFrame = 0;
}

void TimeSystem::Update()
{
	std::uint64_t waitTimeMs = SDL_GetTicks64() + FRAMETIME_MS - (SDL_GetTicks64() - msLastFrame);
	dt = (SDL_GetTicks64() - msLastFrame) / 1000.0f;
	msLastFrame = SDL_GetTicks64();
}
