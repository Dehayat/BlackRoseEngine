#include "Animation/AnimationSystem.h"

#include <entt/entt.hpp>

#include "Entity.h"
#include "AssetStore.h"

#include "TimeSystem.h"
#include "EventSystem/EntityEventSystem.h"

#include "Systems.h"
#include "Events/AnimationEvent.h"

#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"

AnimationPlayer::AnimationPlayer() {
}

void AnimationPlayer::Update() {
	float dt = GETSYSTEM(TimeSystem).GetdeltaTime();
	Entities& entities = GETSYSTEM(Entities);
	AssetStore& assetStore = GETSYSTEM(AssetStore);
	EntityEventSystem& eventSystem = GETSYSTEM(EntityEventSystem);
	entt::registry& registry = entities.GetRegistry();
	auto view = registry.view<AnimationComponent, SpriteComponent>();
	for (auto entity : view) {
		auto& animationComponent = view.get<AnimationComponent>(entity);
		auto& spriteComponent = view.get<SpriteComponent>(entity);
		animationComponent.Update(dt);
		auto animation = assetStore.GetAnimation(animationComponent.animation);
		if (animation == nullptr) {
			continue;
		}
		spriteComponent.sprite = animation->texture;
		if (spriteComponent.sourceRect != nullptr) {
			delete spriteComponent.sourceRect;
		}
		spriteComponent.sourceRect = new SDL_Rect(animation->GetSourceRect(animationComponent.currentFrame));
		while (animationComponent.IsEventQueued()) {
			std::string eventName = animationComponent.PopEvent();
			eventSystem.QueueEvent(EntityEvent(entity, eventName));
		}
		if (animationComponent.JustFinished()) {
			eventSystem.QueueEvent(EntityEvent(entity, "AnimationFinished"));
		}
	}
}