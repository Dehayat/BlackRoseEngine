#pragma once
#include <string>
#include <vector>
#include <queue>

#include "Asset.h"
#include "../Reflection/Reflection.h"
#include "../Core/Guid.h"

#include <SDL2/SDL_rect.h>

class Frame {

public:
	int framePosition = 0;
	float frameDuration = 0;
	Guid id;
	Frame(int framePosition = 0, float frameDuration = 0) {
		this->framePosition = framePosition;
		this->frameDuration = frameDuration;
		id = GuidGenerator::New();
	}
};

class AnimationEventData {

public:
	float eventTime = 0;
	std::string eventName;
	Guid id;
	AnimationEventData(float eventTime = 0, std::string eventName = "NewAction") {
		this->eventTime = eventTime;
		this->eventName = eventName;
		id = GuidGenerator::New();
	}
};

class Animation :public Asset {
	int spriteFrameWidth;
	int spriteFrameHeight;

public:
	std::string texture;
	bool isLooping;
	std::vector<Frame*> frames;
	std::vector<AnimationEventData*> animationEvents;

	Animation(int spriteWidth = 1, int spriteHeight = 1, std::string spriteTexture = "", bool isLooping = false);
	SDL_Rect GetSourceRect(int frame);
	void AddFrame(Frame* frame);
	void AddEvent(AnimationEventData* animationEvent);

	ROSE_EXPOSE_VARS(Animation, (spriteFrameWidth)(spriteFrameHeight)(texture)(isLooping))
};