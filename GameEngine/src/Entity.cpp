#include "Entity.h"
#include "Components/GUIDComponent.h"

#include <entt/entt.hpp>

#include "Systems.h"

#include "Logger.h"

Entities::Entities()
{
	CREATESYSTEM(entt::registry);
	gen = std::mt19937_64(rd());
}

entt::registry& Entities::GetRegistry()
{
	return GETSYSTEM(entt::registry);
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
		auto& regsitry = GetRegistry();
		if (regsitry.valid(allEntities[guid])) {
			return true;
		}
		else {
			return false;
		}
	}
}

void Entities::DestroyAllEntities()
{
	auto& registry = GetRegistry();
	for (auto& it : allEntities)
	{
		if (registry.valid(it.second)) {
			registry.destroy(it.second);
		}
	}
	allEntities.clear();
}

void Entities::AddEntity(Guid guid, entt::entity entity)
{
	allEntities.emplace(guid, entity);
}

entt::entity Entities::CreateEntity()
{
	auto guid = GenerateGuid();
	return CreateEntity(guid);
}
entt::entity Entities::CreateEntity(Guid guid)
{
	auto& registry = GetRegistry();
	auto entity = registry.create();
	registry.emplace<GUIDComponent>(entity, guid);
	AddEntity(guid, entity);
	return entity;
}


void Entities::DestroyEntity(entt::entity entity)
{
	auto& registry = GetRegistry();
	Guid guid = -1;
	if (registry.valid(entity)) {
		guid = registry.get<GUIDComponent>(entity).id;
		registry.destroy(entity);
	}
	if (guid != -1 && allEntities.find(guid) != allEntities.end()) {
		allEntities.erase(guid);
	}
}
