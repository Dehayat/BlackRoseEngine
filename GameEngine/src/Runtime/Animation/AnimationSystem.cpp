#include "Animation/AnimationSystem.h"

#include <entt/entt.hpp>

#include "Core/Entity.h"
#include "AssetPipline/AssetStore.h"
#include "Core/TimeSystem.h"

#include "Events/EntityEventSystem.h"

#include "Core/Systems.h"


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
		if (spriteComponent.sourceRect != nullptr) {
			delete spriteComponent.sourceRect;
			spriteComponent.sourceRect = nullptr;
		}
		animationComponent.Update(dt);
		auto animationHandle = assetStore.GetAsset(animationComponent.animation);
		if (animationHandle.type != AssetType::Animation) {
			continue;
		}
		auto animation = static_cast<Animation*>(animationHandle.asset);
		if (animation == nullptr) {
			continue;
		}
		spriteComponent.sprite = animation->texture;
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