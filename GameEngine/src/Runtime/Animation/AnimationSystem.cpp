#include "Animation/AnimationSystem.h"

#include "Core/Entity.h"
#include "AssetPipline/AssetStore.h"
#include "Core/TimeSystem.h"

#include "Events/EntityEventSystem.h"

#include "Core/Systems.h"


#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/DisableComponent.h"

AnimationPlayer::AnimationPlayer()
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	registry.on_destroy<AnimationComponent>().connect<&AnimationPlayer::AnimationDestroyed>(this);
}

void AnimationPlayer::Update()
{
	float dt = ROSE_GETSYSTEM(TimeSystem).GetdeltaTime();
	Entities& entities = ROSE_GETSYSTEM(Entities);
	AssetStore& assetStore = ROSE_GETSYSTEM(AssetStore);
	EntityEventSystem& eventSystem = ROSE_GETSYSTEM(EntityEventSystem);
	entt::registry& registry = entities.GetRegistry();
	auto view = registry.view<AnimationComponent, SpriteComponent>(entt::exclude<DisableComponent>);
	for(auto entity : view)
	{
		auto& animationComponent = view.get<AnimationComponent>(entity);
		auto& spriteComponent = view.get<SpriteComponent>(entity);
		if(spriteComponent.sourceRect != nullptr)
		{
			delete spriteComponent.sourceRect;
			spriteComponent.sourceRect = nullptr;
		}
		animationComponent.Update(dt);
		auto animationHandle = assetStore.GetAsset(animationComponent.animation);
		if(animationHandle.type != AssetType::Animation)
		{
			continue;
		}
		auto animation = static_cast<Animation*>(animationHandle.asset);
		if(animation == nullptr)
		{
			continue;
		}
		spriteComponent.sprite = animation->texture;
		spriteComponent.sourceRect = new SDL_Rect(animation->GetSourceRect(animationComponent.currentFrame));
		while(animationComponent.IsEventQueued())
		{
			std::string eventName = animationComponent.PopEvent();
			eventSystem.QueueEvent(EntityEvent(entity, eventName));
		}
		if(animationComponent.JustFinished())
		{
			eventSystem.QueueEvent(EntityEvent(entity, "AnimationFinished"));
		}
	}
}

void AnimationPlayer::AnimationDestroyed(entt::registry& registry, entt::entity entity)
{
	if(registry.any_of<SpriteComponent>(entity))
	{
		auto& spriteComponent = registry.get<SpriteComponent>(entity);
		if(spriteComponent.sourceRect != nullptr)
		{
			delete spriteComponent.sourceRect;
			spriteComponent.sourceRect = nullptr;
		}
	}
}
