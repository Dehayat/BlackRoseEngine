#pragma once
#include <unordered_map>
#include "Core/Guid.h"

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <ryml/ryml.hpp>

typedef std::unordered_map <Guid, entt::entity> EntityMap;
typedef std::unordered_map <entt::entity, Guid> GuidMap;

inline static entt::entity NoEntity()
{
	return entt::null;
}

class EntitySystem
{
private:
	EntityMap allEntities;
	GuidMap allEntityGuids;
	entt::registry mainRegistry;

public:
	EntitySystem();
	entt::entity CreateEntity();
	entt::entity DeserializeEntity(ryml::NodeRef& node);
	entt::registry& GetRegistry();
	entt::entity GetEntity(Guid guid);
	Guid GetEntityGuid(entt::entity);
	bool EntityExists(Guid guid);
	bool EntityExists(entt::entity);
	void DestroyAllEntities();
	void DestroyEntity(entt::entity entity);
	void EnableEntity(entt::entity entity);
	void DisableEntity(entt::entity entity);
	entt::entity CopyEntity(entt::entity entity, entt::entity parent = NoEntity());
	template<typename TComponent>
	TComponent& GetComponent(entt::entity entity)
	{
		return GetRegistry().get<TComponent>(entity);
	}
	template<typename TComponent>
	void RemoveComponent(entt::entity entity)
	{
		return GetRegistry().remove<TComponent>(entity);
	}
};