#include "Animation/AnimationAsset.h"

bool Animation::AllEventsDone() {
	return nextEventIndex == animationEvents.size();
}

Animation::Animation(int spriteWidth, int spriteHeight, std::string spriteTexture, bool isLooping) {
	spriteFrameHeight = spriteHeight;
	spriteFrameWidth = spriteWidth;
	currentFrameTime = 0;
	nextEventIndex = 0;
	currentAnimationTime = 0;
	texture = spriteTexture;
	currentFrame = 0;
	this->isLooping = isLooping;
	justFinished = false;
	isOver = false;
}

SDL_Rect Animation::GetSourceRect() {
	SDL_Rect rect{
		frames[currentFrame]->framePosition * spriteFrameWidth,
		0,
		spriteFrameWidth,
		spriteFrameHeight
	};
	return rect;
}

void Animation::Update(float dt) {
	currentFrameTime += dt;
	currentAnimationTime += dt;
	while (!AllEventsDone()) {
		if (currentAnimationTime >= animationEvents[nextEventIndex]->eventTime) {
			eventQueue.push(animationEvents[nextEventIndex]->eventName);
			nextEventIndex++;
		}
		else {
			break;
		}
	}

	justFinished = false;
	if (currentFrameTime >= frames[currentFrame]->frameDuration) {
		if (currentFrame < frames.size() - 1) {
			currentFrameTime = 0;
			currentFrame++;
		}
		else {
			if (!isOver) {
				justFinished = true;
			}
			if (isLooping) {
				currentFrameTime = 0;
				currentFrame++;
				currentFrame %= frames.size();
			}
			else {
				isOver = true;
			}
		}
	}
}

void Animation::Reset() {
	currentFrame = 0;
	currentFrameTime = 0;
	justFinished = false;
	isOver = false;
	nextEventIndex = 0;
	currentAnimationTime = 0;
	while (!eventQueue.empty()) {
		eventQueue.pop();
	}
}

void Animation::AddFrame(Frame* frame) {
	frames.push_back(frame);
}

void Animation::AddEvent(AnimationEventData* animationEvent) {
	animationEvents.push_back(animationEvent);
}

bool Animation::JustFinished() {
	return justFinished;
}

bool Animation::IsEventQueued() {
	return !eventQueue.empty();
}

std::string Animation::PopEvent() {
	auto& eventName = eventQueue.front();
	eventQueue.pop();
	return eventName;
}
