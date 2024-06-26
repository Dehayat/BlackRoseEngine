#include "Core/Entity.h"

#include <entt/entity/registry.hpp>

#include "LevelTree.h"
#include "Core/Systems.h"
#include "EntitySerializer.h"
#include "Components/GUIDComponent.h"

EntitySystem::EntitySystem()
{
	mainRegistry = entt::registry();
	ROSE_CREATESYSTEM(LevelTree);
}

entt::registry& EntitySystem::GetRegistry()
{
	return mainRegistry;
}

entt::entity EntitySystem::GetEntity(Guid guid)
{
	if(allEntities.find(guid) != allEntities.end())
	{
		return allEntities[guid];
	} else
	{
		return NoEntity();
	}
}

Guid EntitySystem::GetEntityGuid(entt::entity entity)
{
	return allEntityGuids[entity];
}

bool EntitySystem::EntityExists(Guid guid)
{
	if(allEntities.find(guid) == allEntities.end())
	{
		return false;
	} else
	{
		auto& regsitry = GetRegistry();
		if(regsitry.valid(allEntities[guid]))
		{
			return true;
		} else
		{
			return false;
		}
	}
}
bool EntitySystem::EntityExists(entt::entity entity)
{
	return GetRegistry().valid(entity);
}

void EntitySystem::DestroyAllEntities()
{
	auto& registry = GetRegistry();
	for(auto& it : allEntities)
	{
		if(registry.valid(it.second))
		{
			registry.destroy(it.second);
		}
	}
	allEntities.clear();
	allEntityGuids.clear();
}
entt::entity EntitySystem::CreateEntity()
{
	auto& registry = GetRegistry();
	auto entity = registry.create();
	auto& guidComp = registry.emplace<GUIDComponent>(entity);
	auto guid = guidComp.id;
	allEntities.emplace(guid, entity);
	allEntityGuids.emplace(entity, guid);
	ROSE_GETSYSTEM(LevelTree).AddEntity(entity);
	return entity;
}
entt::entity EntitySystem::DeserializeEntity(ryml::NodeRef& node)
{
	auto& registry = GetRegistry();
	auto entity = registry.create();
	if(node.has_child("Guid"))
	{
		auto guid = GuidGenerator::New();
		auto guidNode = node["Guid"];
		if(guidNode.has_child("id"))
		{
			guidNode["id"] >> guid;
		}
		ComponentSer<GUIDComponent>::Deserialize(registry, guidNode, entity);
		auto& guidComp = registry.get<GUIDComponent>(entity);
		guidComp.id = guid;
		allEntities.emplace(guid, entity);
		allEntityGuids.emplace(entity, guid);
		ROSE_GETSYSTEM(LevelTree).AddEntity(entity);

	} else
	{
		entity = CreateEntity();
	}

	auto& guidComp = registry.get<GUIDComponent>(entity);
	if(EntityExists(guidComp.parentId))
	{
		ROSE_GETSYSTEM(LevelTree).TrySetParent(entity, GetEntity(guidComp.parentId));
	}
	EntitySerializer::DeserializeEntity(node, registry, entity);
	return entity;
}

entt::entity EntitySystem::Copy(entt::entity src)
{
	auto& registry = GetRegistry();
	auto entity = registry.create();
	for(auto [id, storage] : registry.storage())
	{
		if(storage.contains(src))
		{
			storage.emplace(entity, storage.get(src));
		}
	}
	auto& guidComp = registry.get_or_emplace<GUIDComponent>(entity);
	auto guid = GuidGenerator::New();
	guidComp.id = guid;
	allEntities.emplace(guid, entity);
	allEntityGuids.emplace(entity, guid);
	ROSE_GETSYSTEM(LevelTree).AddEntity(entity);
	return entity;
}

void EntitySystem::DestroyEntity(entt::entity entity)
{
	auto& registry = GetRegistry();
	Guid guid = -1;
	if(registry.valid(entity))
	{
		guid = registry.get<GUIDComponent>(entity).id;
		registry.destroy(entity);
	}
	if(guid != -1 && allEntities.find(guid) != allEntities.end())
	{
		allEntities.erase(guid);
	}
	if(allEntityGuids.find(entity) != allEntityGuids.end())
	{
		allEntityGuids.erase(entity);
	}
}
