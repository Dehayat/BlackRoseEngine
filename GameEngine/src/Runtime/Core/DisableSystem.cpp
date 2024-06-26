#include "DisableSystem.h"

#include "Core/Systems.h"
#include "Core/Log.h"

#include "Core/LevelTree.h"

#include "Components/DisableComponent.h"


DisableSystem::DisableSystem()
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	registry.on_construct<DisableComponent>().connect<&DisableSystem::DisableCreated>(this);
	registry.on_destroy<DisableComponent>().connect<&DisableSystem::DisableDestroyed>(this);
}
void DisableSystem::Enable(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	if(registry.any_of<DisableComponent>(entity))
	{
		auto& disable = registry.get<DisableComponent>(entity);
		disable.selfDisabled = false;
		if(!disable.parentDisabled)
		{
			registry.remove<DisableComponent>(entity);
		}
	}
}
void DisableSystem::Disable(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& disable = registry.get_or_emplace<DisableComponent>(entity);
	disable.selfDisabled = true;
}
void DisableSystem::DisableCreated(entt::registry& registry, entt::entity entity)
{
	DisableChildren(entity);
}

void DisableSystem::DisableDestroyed(entt::registry& registry, entt::entity entity)
{
	EnableChildren(entity);
}

void DisableSystem::DisableChildren(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& levelTree = ROSE_GETSYSTEM(LevelTree);
	if(levelTree.GetNode(entity) != nullptr)
	{
		for(const auto& child : levelTree.GetNode(entity)->children)
		{
			auto& childDisable = registry.get_or_emplace<DisableComponent>(child->element, false, false);
			childDisable.parentDisabled = true;
		}
	}
}
void DisableSystem::EnableChildren(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& levelTree = ROSE_GETSYSTEM(LevelTree);
	if(levelTree.GetNode(entity) != nullptr)
	{
		for(auto child : levelTree.GetNode(entity)->children)
		{
			auto& childDisable = registry.get<DisableComponent>(child->element);
			if(!childDisable.selfDisabled)
			{
				registry.remove<DisableComponent>(child->element);
			}
		}
	}
}