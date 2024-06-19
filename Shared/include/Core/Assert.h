#pragma once
#include <SDL2/SDL_assert.h>

#ifdef _DEBUG
#define ROSE_ASSERT(condition) SDL_assert(condition)
#else
#define ROSE_ASSERT(condition)
#endif // _DEBUG