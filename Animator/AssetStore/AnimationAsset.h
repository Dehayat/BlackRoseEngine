#pragma once
#include <string>
#include <vector>
#include <queue>

#include "../Entity.h"
#include "AssetHandle.h"
#include "../Systems.h"

#include <SDL2/SDL.h>

class Frame {

public:
	int framePosition = 0;
	float frameDuration = 0;
	Guid id;
	Frame(int framePosition = 0, float frameDuration = 0) {
		this->framePosition = framePosition;
		this->frameDuration = frameDuration;
		id = GETSYSTEM(Entities).GenerateGuid();
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
		id = GETSYSTEM(Entities).GenerateGuid();
	}
};

class Animation :public Asset {

public:
	int spriteFrameWidth;
	int spriteFrameHeight;
	std::string texture;
	bool isLooping;
	std::vector<Frame*> frames;
	std::vector<AnimationEventData*> animationEvents;

	Animation(int spriteWidth, int spriteHeight, std::string spriteTexture, bool isLooping = false);
	SDL_Rect GetSourceRect(int frame);
	void AddFrame(Frame* frame);
	void AddEvent(AnimationEventData* animationEvent);
};