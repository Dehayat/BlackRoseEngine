#include "Core/TimeSystem.h"
#include <cstdint>

#include <SDL2/SDL.h>

#include "Debugging/Logger.h"

const float MAX_DT = FRAMETIME_MS / 1000.0 * 4;

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
	if (dt > MAX_DT) {
		dt = MAX_DT;
	}
	msLastFrame = SDL_GetTicks64();
}

float TimeSystem::GetdeltaTime()
{
	return dt;
}
