#pragma once
#include <string>
#include <vector>
#include <queue>

#include "AssetStore/AssetHandle.h"

#include <SDL2/SDL.h>

class Frame {

public:
	const int framePosition = 0;
	const float frameDuration = 0;
};

class AnimationEventData {

public:
	const float eventTime = 0;
	const std::string eventName;
};

class Animation :public Asset {

private:
	const int spriteFrameWidth;
	const int spriteFrameHeight;


public:
	const std::string texture;
	const bool isLooping;
	std::vector<Frame*> frames;
	std::vector<AnimationEventData*> animationEvents;

	Animation(int spriteWidth, int spriteHeight, std::string spriteTexture, bool isLooping = false);
	SDL_Rect GetSourceRect(int frame);
	void AddFrame(Frame* frame);
	void AddEvent(AnimationEventData* animationEvent);
};