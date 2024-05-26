#pragma once
#pragma once
#include <string>

#include <SDL2/SDL.h>

#include "EventSystem/EventBus.h"
#include "Animation/AnimationAsset.h"

#include "Components/AnimationComponent.h"

class AnimationPlayer {

private:
	EventBus* eventBus;

public:
	AnimationPlayer();
	void SetEventBus(EventBus* eventBus);
	void Update();
};