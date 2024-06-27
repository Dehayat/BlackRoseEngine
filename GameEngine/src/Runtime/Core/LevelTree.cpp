#include "LevelTree.h"

#include <entt/entity/registry.hpp>

#include "Core/Systems.h"
#include "Core/Transform.h"

#include "Components/GUIDComponent.h"

#include "Core/Log.h"
#include <Core/Assert.h>

LevelTree::LevelTree(entt::entity rootEntity)
{
	root = new Node<entt::entity>(rootEntity);
	nodesMap[root->element] = root;
}
LevelTree::~LevelTree()
{
	if(root != nullptr && root->element == NoEntity())
	{
		delete root;
		root = nullptr;
	}
}
Node<entt::entity>* LevelTree::GetParentNode(entt::entity entity)
{
	auto& entities = ROSE_GETSYSTEM(EntitySystem);
	auto guidComp = entities.GetRegistry().get<GUIDComponent>(entity);
	return nodesMap[guidComp.parent];
}
void LevelTree::RemoveParent(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& childGuid = registry.get<GUIDComponent>(entity);
	if(childGuid.parent != NoEntity())
	{
		auto& childTrx = registry.get<TransformComponent>(entity);
		auto& transformSystem = ROSE_GETSYSTEM(TransformSystem);
		childGuid.parent = NoEntity();
		childGuid.parentId = -1;
		root->AddChild(nodesMap[entity]);
		registry.patch<GUIDComponent>(entity);
		ROSE_LOG("removing parent");
	}
}
bool LevelTree::TrySetParent(entt::entity child, entt::entity parent)
{
	if(IsChildOf(child, parent))
	{
		return false;
	}
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& childGuid = registry.get<GUIDComponent>(child);

	if(childGuid.parent != NoEntity())
	{
		RemoveParent(child);
	}

	childGuid.parent = parent;
	childGuid.parentId = ROSE_GETSYSTEM(EntitySystem).GetEntityGuid(parent);
	auto node = nodesMap[child];
	nodesMap[parent]->AddChild(node);
	registry.patch<GUIDComponent>(child);
	ROSE_LOG("parent set");
	return true;
}
bool LevelTree::IsChildOf(entt::entity entity, entt::entity child)
{
	auto node = nodesMap[entity];
	auto childNode = nodesMap[child];

	if(node->children.find(childNode) != node->children.end())
	{
		return true;
	} else
	{
		for(auto childNode : node->children)
		{
			if(IsChildOf(childNode->element, child))
			{
				return true;
			}
		}
	}
	return false;
}
void LevelTree::Clear()
{
	root->DeleteChildren();
	nodesMap.clear();
}
Node<entt::entity>* LevelTree::GetRoot()
{
	return root;
}
entt::entity LevelTree::GetChild(entt::entity entity, const std::string& name)
{
	auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	if(!registry.valid(entity))
	{
		return NoEntity();
	}
	if(nodesMap.find(entity) == nodesMap.end())
	{
		return NoEntity();
	}
	auto node = nodesMap[entity];
	for(auto child : node->children)
	{
		if(!registry.valid(child->element))
		{
			continue;
		}
		if(registry.get<GUIDComponent>(child->element).name == name)
		{
			return child->element;
		}
	}
	return NoEntity();
}
Node<entt::entity>* LevelTree::GetNode(entt::entity entity)
{
	if(nodesMap.find(entity) == nodesMap.end())
	{
		return nullptr;
	}
	return nodesMap[entity];
}
Node<entt::entity>* LevelTree::AddEntity(entt::entity entity, Node<entt::entity>* parent)
{
	if(parent == nullptr)
	{
		parent = root;
	}
	auto node = new Node<entt::entity>(entity, parent);
	nodesMap[entity] = node;
	return node;
}

void LevelTree::RemoveEntity(entt::entity entity)
{
	root->RemoveChild(nodesMap[entity]);
	delete nodesMap[entity];
	nodesMap.erase(entity);
}
