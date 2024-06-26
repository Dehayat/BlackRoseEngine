#pragma once
#include <string>

#include "AnimationAsset.h"

class AnimationImporter {
private:
	static std::string ReadToEndOfLine(const std::string& fileString, int startPos);
	static std::string ReadToSpace(const std::string& fileString, int startPos);
public:
	static Animation* LoadAnimation(const std::string& fileName);
	static bool SaveAnimation(Animation* animation, const std::string& fileName);
};