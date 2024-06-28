#include "Core/Entity.h"

#include <entt/entity/registry.hpp>

#include "LevelTree.h"
#include "Core/Systems.h"
#include "EntitySerializer.h"
#include "DisableSystem.h"

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
	if(guid == -1)
	{
		return NoEntity();
	}
	return allEntities[guid];
}

Guid EntitySystem::GetEntityGuid(entt::entity entity)
{
	if(entity == NoEntity())
	{
		return -1;
	}
	return allEntityGuids[entity];
}

bool EntitySystem::EntityExists(Guid guid)
{
	if(allEntities.find(guid) == allEntities.end())
	{
		return false;
	} else
	{
		return EntityExists(allEntities[guid]);
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
	ROSE_GETSYSTEM(LevelTree).Clear();
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

entt::entity EntitySystem::CopyEntity(entt::entity src, entt::entity parent)
{
	auto& registry = GetRegistry();
	auto entity = CreateEntity();
	auto& guidComp = registry.get<GUIDComponent>(entity);
	auto& srcGuidComp = registry.get<GUIDComponent>(src);
	guidComp.name = srcGuidComp.name;
	if(parent != NoEntity())
	{
		ROSE_GETSYSTEM(LevelTree).TrySetParent(entity, parent);
	}
	for(auto [id, storage] : registry.storage())
	{
		if(storage.contains(src) && !storage.contains(entity))
		{
			storage.emplace(entity, storage.get(src));
		}
	}
	for(auto child : ROSE_GETSYSTEM(LevelTree).GetNode(src)->children)
	{
		auto childEntity = CopyEntity(child->element, entity);
	}
	return entity;
}

void EntitySystem::EnableEntity(entt::entity entity)
{
	ROSE_GETSYSTEM(DisableSystem).Enable(entity);
}

void EntitySystem::DisableEntity(entt::entity entity)
{
	ROSE_GETSYSTEM(DisableSystem).Disable(entity);
}


void EntitySystem::DestroyEntity(entt::entity entity)
{
	auto& registry = GetRegistry();
	if(EntityExists(entity))
	{
		for(auto child : ROSE_GETSYSTEM(LevelTree).GetNode(entity)->children)
		{
			DestroyEntity(child->element);
		}
		ROSE_GETSYSTEM(LevelTree).RemoveEntity(entity);
		auto guid = allEntityGuids[entity];
		allEntities.erase(guid);
		allEntityGuids.erase(entity);
		registry.destroy(entity);
	}
}
