#pragma once
#include <string>
#include <queue>

#include "Systems.h"

#include "AssetStore/AssetStore.h"
#include "AssetStore/AnimationAsset.h"

#include <SDL2/SDL_rect.h>

struct AnimationComponent {
	std::string animation;

	float currentFrameTime;
	int currentFrame;
	bool justFinished;
	bool isOver;
	int nextEventIndex;
	float currentAnimationTime;
	std::queue<std::string> eventQueue;

	void Reset() {
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
	AnimationComponent(std::string animaiton = "") {
		this->animation = animaiton;
		Reset();
	}

	void SetTime(float t) {
		currentAnimationTime = t;
		AssetStore& assetStore = GETSYSTEM(AssetStore);
		auto animationHandle = assetStore.GetAsset(animation);
		auto animationAsset = static_cast<Animation*>(animationHandle.asset);
		if (animationAsset == nullptr || animationAsset->frames.size() == 0) {
			return;
		}
		float pastFramesTime = 0;
		currentFrame = 0;
		for (auto frame : animationAsset->frames) {
			if (pastFramesTime + frame->frameDuration > t) {
				break;
			}
			currentFrame++;
			pastFramesTime += frame->frameDuration;
		}
		if (currentFrame >= animationAsset->frames.size()) {
			currentFrame = animationAsset->frames.size() - 1;
			currentFrameTime = animationAsset->frames[currentFrame]->frameDuration;
			currentAnimationTime = t;
		}
		else {
			currentFrameTime = t - pastFramesTime;
		}
	}

	void Update(float dt) {
		AssetStore& assetStore = GETSYSTEM(AssetStore);
		auto animationHandle = assetStore.GetAsset(animation);
		if (animationHandle.type != AssetType::Animation) {
			return;
		}
		auto animationAsset = static_cast<Animation*>(animationHandle.asset);
		if (animationAsset == nullptr) {
			return;
		}
		if (animationAsset->frames.size() == 0) {
			return;
		}
		if (currentFrame >= animationAsset->frames.size()) {
			Reset();
		}
		currentFrameTime += dt;
		currentAnimationTime += dt;
		while (nextEventIndex < animationAsset->animationEvents.size()) {
			if (currentAnimationTime >= animationAsset->animationEvents[nextEventIndex]->eventTime) {
				eventQueue.push(animationAsset->animationEvents[nextEventIndex]->eventName);
				nextEventIndex++;
			}
			else {
				break;
			}
		}

		justFinished = false;
		if (currentFrameTime >= animationAsset->frames[currentFrame]->frameDuration) {
			if (currentFrame < animationAsset->frames.size() - 1) {
				currentFrameTime = 0;
				currentFrame++;
			}
			else {
				if (!isOver) {
					justFinished = true;
				}
				if (animationAsset->isLooping) {
					currentFrameTime = 0;
					currentFrame++;
					currentFrame %= animationAsset->frames.size();
				}
				else {
					isOver = true;
				}
			}
		}
	}

	bool JustFinished() const {
		return justFinished;
	}

	bool IsEventQueued() const {
		return !eventQueue.empty();
	}

	std::string PopEvent() {
		auto& eventName = eventQueue.front();
		eventQueue.pop();
		return eventName;
	}

	void Play(const std::string& animation) {
		this->animation = animation;
		Reset();
	}

	float GetAnimationDuration() {
		AssetStore& assetStore = GETSYSTEM(AssetStore);
		auto animationHandle = assetStore.GetAsset(animation);
		auto animationAsset = static_cast<Animation*>(animationHandle.asset);
		if (animationAsset == nullptr) {
			return 1;
		}
		float pastFramesTime = 0;
		for (auto frame : animationAsset->frames) {
			pastFramesTime += frame->frameDuration;
		}
		return pastFramesTime;
	}

};