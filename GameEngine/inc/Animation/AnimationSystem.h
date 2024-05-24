#pragma once
#pragma once
#include <string>

#include <SDL2/SDL.h>

#include "EventSystem/EventBus.h"
#include "Animation/AnimationAsset.h"

class AnimationPlayer {

private:
	Animation* currentAnimation;
	EventBus* eventBus;

public:
	AnimationPlayer();
	void SetEventBus(EventBus* eventBus);
	void Update();
};