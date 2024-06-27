#pragma once
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

class DisableSystem
{
	void DisableCreated(entt::registry& registry, entt::entity);
	void DisableDestroyed(entt::registry& registry, entt::entity);
	void ParentUpdated(entt::registry& registry, entt::entity);
	void EnableChild(entt::entity entity);
	void DisableChildren(entt::entity entity);
	void DisableChild(entt::entity entity);
	void EnableChildren(entt::entity entity);
public:
	DisableSystem();
	void Enable(entt::entity entity);
	void Disable(entt::entity entity);
};