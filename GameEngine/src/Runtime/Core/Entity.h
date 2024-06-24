#pragma once
#include <unordered_map>
#include "Core/Guid.h"

#include <entt/entity/entity.hpp>

typedef std::unordered_map <Guid, entt::entity> EntityMap;
typedef std::unordered_map <entt::entity, Guid> GuidMap;

inline static entt::entity NoEntity()
{
	return entt::null;
}

class Entities
{
private:
	EntityMap allEntities;
	GuidMap allEntityGuids;

public:
	Entities();
	entt::registry& GetRegistry();
	entt::entity GetEntity(Guid guid);
	Guid GetEntityGuid(entt::entity);
	bool EntityExists(Guid guid);
	bool EntityExists(entt::entity);
	void DestroyAllEntities();
	void AddEntity(Guid guid, entt::entity entity);
	entt::entity CreateEntity();
	entt::entity CreateEntity(Guid guid);
	entt::entity CreateEntityWithoutGuidComponent(Guid guid);
	void DestroyEntity(entt::entity entity);
	entt::entity Copy(entt::entity entity);
};