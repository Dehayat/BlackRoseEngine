#pragma once
#include <string>
#include <vector>
#include <queue>

#include <SDL2/SDL.h>

class Frame {

public:
	int framePosition = 0;
	float frameDuration = 0;
};

class AnimationEventData {

public:
	float eventTime = 0;
	std::string eventName;
};

class Animation {

private:
	int spriteFrameWidth;
	int spriteFrameHeight;


public:
	std::string texture;
	bool isLooping;
	std::vector<Frame*> frames;
	std::vector<AnimationEventData*> animationEvents;

	Animation(int spriteWidth, int spriteHeight, std::string spriteTexture, bool isLooping = false);
	SDL_Rect GetSourceRect(int frame);
	void AddFrame(Frame* frame);
	void AddEvent(AnimationEventData* animationEvent);
};