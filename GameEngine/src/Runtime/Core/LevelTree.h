#pragma once

#include <entt/entity/entity.hpp>

#include "Structures/Tree.h"

#include "Core/Entity.h"

#include "Components/TransformComponent.h"

class LevelTree
{
	Node<entt::entity>* root;
	std::unordered_map<entt::entity, Node<entt::entity>*> nodesMap;

	Node<entt::entity>* GetParentNode(entt::entity trx);

public:
	Node<entt::entity>* AddEntity(entt::entity entity, Node<entt::entity>* parent = nullptr);
	void RemoveEntity(entt::entity entity);
	LevelTree(entt::entity rootEntity = NoEntity());
	~LevelTree();
	void RemoveParent(entt::entity);
	bool TrySetParent(entt::entity child, entt::entity parent);
	bool IsChildOf(entt::entity entity, entt::entity child);
	void Clear();
	Node<entt::entity>* GetRoot();
	entt::entity GetChild(entt::entity entity, const std::string& name);
	Node<entt::entity>* GetNode(entt::entity entity);
};