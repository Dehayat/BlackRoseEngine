#pragma once
#include <string>

#include "Systems.h"

#include "AssetStore/AssetStore.h"

#include <ryml/ryml.hpp>
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

	AnimationComponent(ryml::NodeRef node)
	{
		Reset();
		this->animation = "";
		if (node.is_map()) {
			if (node.has_child("animation")) {
				node["animation"] >> this->animation;
			}
		}
	}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["animation"] << animation;
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
				currentFrame %= animationAsset->frames.size();
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
		auto eventName = eventQueue.front();
		eventQueue.pop();
		return eventName;
	}

	void Play(const std::string& animation) {
		this->animation = animation;
		Reset();
	}

};