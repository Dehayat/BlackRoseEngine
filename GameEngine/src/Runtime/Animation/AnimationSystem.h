#pragma once
#include <string>

#include <SDL2/SDL.h>
#include <entt/entt.hpp>

#include "Events/EventBus.h"

#include "AssetPipline/AnimationAsset.h"

#include "Components/AnimationComponent.h"

class AnimationSystem {

public:
	AnimationSystem();
	void Update();
	void AnimationDestroyed(entt::registry& registry, entt::entity entity);
};