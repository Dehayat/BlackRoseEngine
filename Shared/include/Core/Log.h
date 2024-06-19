#pragma once
#include <SDL2/SDL_log.h>

#ifdef _DEBUG

#define ROSE_LOG(format,...)  SDL_Log(format,__VA_ARGS__)
#define ROSE_ERR(format,...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, format,__VA_ARGS__);\
							SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s:%d",__FILE__, __LINE__)

#else

#define ROSE_LOG(format,...)
#define ROSE_ERR(format,...)

#endif // _DEBUG