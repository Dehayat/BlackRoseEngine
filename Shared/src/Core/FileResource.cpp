#include "FileResource.h"

#include "Assert.h"
#include "Log.h"

FileResource::FileResource(const std::string& fileName, const std::string how) {
	ROSE_ASSERT(!fileName.empty());
	ROSE_ASSERT(how == "r" || how == "w" || how == "w+");
	file = SDL_RWFromFile(fileName.c_str(), how.c_str());
	if (file == nullptr) {
		ROSE_ERR("couldnt open file: %s", fileName.c_str());
	}
}

FileResource::~FileResource() {
	if (file != nullptr) {
		file->close(file);
	}
	else {
		ROSE_ERR("no file to close");
	}
}
