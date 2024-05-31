#pragma once

#include <string>

#include <SDL2/SDL.h>

struct FileResource {
	SDL_RWops* file;
	FileResource(const std::string& fileName, const std::string how = "r") {
		file = SDL_RWFromFile(fileName.c_str(), how.c_str());
	}
	~FileResource() {
		if (file != nullptr) {
			file->close(file);
		}
	}
};