#include "Animation/AnimationSystem.h"

#include <entt/entt.hpp>

#include "Entity.h"
#include "AssetStore.h"

#include "TimeSystem.h"

#include "Systems.h"
#include "Events/AnimationEvent.h"

#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"

AnimationPlayer::AnimationPlayer() {
	currentAnimation = nullptr;
	eventBus = nullptr;
}

void AnimationPlayer::SetEventBus(EventBus* eventBus) {
	this->eventBus = eventBus;
}

void AnimationPlayer::Update() {
	float dt = GETSYSTEM(TimeSystem).GetdeltaTime();
	Entities& entities = GETSYSTEM(Entities);
	AssetStore& assetStore = GETSYSTEM(AssetStore);
	entt::registry& registry = entities.GetRegistry();
	auto view = registry.view<AnimationComponent, SpriteComponent>();
	for (auto entity : view) {
		auto& animationComponent = view.get<AnimationComponent>(entity);
		auto& spriteComponent = view.get<SpriteComponent>(entity);
		auto animation = assetStore.GetAnimation(animationComponent.animation);
		animation->Update(dt);
		spriteComponent.sprite = animation->texture;
		if (spriteComponent.sourceRect != nullptr) {
			delete spriteComponent.sourceRect;
		}
		spriteComponent.sourceRect = new SDL_Rect(animation->GetSourceRect());
		while (animation->IsEventQueued()) {
			std::string eventName = animation->PopEvent();
			eventBus->EmitEvent<AnimationEvent>(eventName);
		}
		if (animation->JustFinished() && eventBus != nullptr) {
			eventBus->EmitEvent<AnimationEvent>("AnimationFinished");
		}
	}
}