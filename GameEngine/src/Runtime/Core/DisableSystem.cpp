#include "DisableSystem.h"

#include "Core/Systems.h"
#include "Core/Log.h"

#include "Editor/LevelTree.h"

#include "Components/DisableComponent.h"


DisableSystem::DisableSystem()
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	registry.on_construct<DisableComponent>().connect<&DisableSystem::DisableCreated>(this);
	registry.on_destroy<DisableComponent>().connect<&DisableSystem::DisableDestroyed>(this);
}
void DisableSystem::Enable(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
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
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
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
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	auto& levelTree = ROSE_GETSYSTEM(LevelTree);
	for(const auto& child : levelTree.GetNode(entity)->children)
	{
		ROSE_LOG("1-%d", registry.valid(child->element));
		ROSE_LOG("2-%d", registry.any_of<DisableComponent>(child->element));
		auto& childDisable = registry.get_or_emplace<DisableComponent>(child->element, false, false);
		childDisable.parentDisabled = true;
	}
}
void DisableSystem::EnableChildren(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	auto& levelTree = ROSE_GETSYSTEM(LevelTree);
	for(auto child : levelTree.GetNode(entity)->children)
	{
		auto& childDisable = registry.get<DisableComponent>(child->element);
		if(!childDisable.selfDisabled)
		{
			registry.remove<DisableComponent>(child->element);
		}
	}
}