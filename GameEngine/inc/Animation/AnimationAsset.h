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
	float currentFrameTime;
	int spriteFrameWidth;
	int spriteFrameHeight;
	std::vector<Frame*> frames;
	std::vector<AnimationEventData*> animationEvents;
	std::queue<std::string> eventQueue;
	int currentFrame;
	bool justFinished;
	bool isOver;
	int nextEventIndex;
	float currentAnimationTime;

	bool AllEventsDone();

public:
	std::string texture;
	bool isLooping;

	Animation(int spriteWidth, int spriteHeight, std::string spriteTexture, bool isLooping = false);
	SDL_Rect GetSourceRect();
	void Update(float dt);
	void Reset();
	void AddFrame(Frame* frame);
	void AddEvent(AnimationEventData* animationEvent);
	bool JustFinished();
	bool IsEventQueued();
	std::string PopEvent();
};