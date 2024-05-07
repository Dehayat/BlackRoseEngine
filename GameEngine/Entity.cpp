#include "Entity.h"
#include <entt/entt.hpp>

entt::registry& Entities::GetRegistry()
{
	entt::registry& registry = entt::locator<entt::registry>::value();
	return registry;
}

entt::entity Entities::GetEntity(Guid guid)
{
	return allEntities[guid];
}

bool Entities::EntityExists(Guid guid)
{
	if (allEntities.find(guid) == allEntities.end()) {
		return false;
	}
	else {
		return true;
	}
}

void Entities::DeleteAllEntities()
{
	allEntities.clear();
}

void Entities::AddEntity(Guid guid, entt::entity entity)
{
	allEntities.emplace(guid, entity);
}
