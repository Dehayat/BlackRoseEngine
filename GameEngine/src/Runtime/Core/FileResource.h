#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "Core/Assert.h"
#include "Core/Log.h"

struct FileResource {
	SDL_RWops* file;
	FileResource(const std::string& fileName, const std::string how = "r") {
		ROSE_ASSERT(!fileName.empty());
		ROSE_ASSERT(how == "r" || how == "w" || how == "w+");
		file = SDL_RWFromFile(fileName.c_str(), how.c_str());
		if (file == nullptr) {
			ROSE_ERR("couldnt open file: %s", fileName.c_str());
		}
	}
	~FileResource() {
		if (file != nullptr) {
			file->close(file);
		}
		else {
			ROSE_ERR("no file to close");
		}
	}
};