#pragma once
#include <string>

#include <SDL2/SDL.h>

#include "Events/EventBus.h"

#include "AssetStore/AnimationAsset.h"

#include "Components/AnimationComponent.h"

class AnimationPlayer {

public:
	AnimationPlayer();
	void Update();
};