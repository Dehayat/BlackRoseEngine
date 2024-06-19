#pragma once

#include <string>

#include <SDL2/SDL.h>

struct FileResource {
	SDL_RWops* file;
	FileResource(const std::string& fileName, const std::string how = "r");
	~FileResource();
};