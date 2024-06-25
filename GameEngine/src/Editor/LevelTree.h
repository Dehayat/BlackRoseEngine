#pragma once

#include <entt/entity/entity.hpp>

#include "Structures/Tree.h"

#include "Core/Entity.h"

#include "Components/TransformComponent.h"

class LevelTree
{
	Node<entt::entity>* root;
	std::unordered_map<entt::entity, Node<entt::entity>*> nodesMap;

	void UpdateChildrenRecursive(entt::registry& registry, Node<entt::entity>* parent);
	Node<entt::entity>* GetParent(TransformComponent& trx);

public:
	Node<entt::entity>* AddEntity(entt::entity entity, Node<entt::entity>* parent = nullptr);
	LevelTree(entt::entity rootEntity = NoEntity());
	~LevelTree();
	void InsertEntity(entt::entity antity);
	void TransformDestroyed(entt::registry& registry, entt::entity entity);
	void RemoveParent(entt::entity);
	bool TrySetParent(entt::entity child, entt::entity parent);
	bool IsChildOf(entt::entity entity, entt::entity child);
	void Clear();
	Node<entt::entity>* GetRoot();
	void CleanTree();
	entt::entity GetChild(entt::entity entity, const std::string& name);
	Node<entt::entity>* GetNode(entt::entity entity);
};