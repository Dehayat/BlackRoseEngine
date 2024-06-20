#include "Editor/LevelTree.h"

#include <entt/entity/registry.hpp>

#include "Core/Transform.h"

#include "Core/Log.h"

LevelTree::LevelTree() {
	root = new Node<entt::entity>(NoEntity());
	nodesMap[entt::entity(-1)] = root;
}
LevelTree::~LevelTree() {
	delete root;
}
void LevelTree::InsertEntity(entt::entity entity)
{
	if (nodesMap.find(entity) != nodesMap.end()) {
		ROSE_ERR("trying to add entity that already exists in levelTree");
		return;
	}
	auto& entities = ROSE_GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	auto& trx = registry.get<TransformComponent>(entity);
	trx.level = 0;
	auto parent = TryGetParent(trx);
	auto node = AddEntity(entity, parent);
	if (parent != nullptr) {
		auto& childTrx = registry.get<TransformComponent>(node->element);
		childTrx.hasParent = true;
		auto& parentTrx = registry.get<TransformComponent>(childTrx.parent);
		childTrx.level = parentTrx.level + 1;
	}
	else if (trx.parentGUID != -1) {
		waitingForParent[trx.parentGUID].push_back(node);
	}

	auto& transformSystem = ROSE_GETSYSTEM(TransformSystem);
	trx.CalcMatrix();

	auto guid = ROSE_GETSYSTEM(Entities).GetEntityGuid(entity);
	ConnectWaitingChildren(node, guid);
}
Node<entt::entity>* LevelTree::TryGetParent(TransformComponent& trx)
{
	auto& entities = ROSE_GETSYSTEM(Entities);
	if (entities.EntityExists(trx.parent)) {
		trx.parentGUID = entities.GetEntityGuid(trx.parent);
	}
	if (entities.EntityExists(trx.parentGUID)) {
		trx.parent = entities.GetEntity(trx.parentGUID);
	}
	if (entities.EntityExists(trx.parent)) {
		if (nodesMap.find(trx.parent) != nodesMap.end()) {
			ROSE_LOG("parent of new entity not found in levelTree");
			return nodesMap[trx.parent];
		}
	}
	return nullptr;
}
void LevelTree::ConnectWaitingChildren(Node<entt::entity>* parentNode, Guid parentGuid)
{
	auto& entities = ROSE_GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	if (waitingForParent.find(parentGuid) != waitingForParent.end()) {
		ROSE_LOG("found parent");
		for (auto child : waitingForParent[parentGuid]) {
			parentNode->AddChild(child);
			auto& childTrx = registry.get<TransformComponent>(child->element);
			childTrx.parent = parentNode->element;
			childTrx.hasParent = true;
		}
		waitingForParent.erase(parentGuid);
		UpdateChildrenRecursive(registry, parentNode);
	}
}
void LevelTree::UpdateChildrenRecursive(entt::registry& registry, Node<entt::entity>* parent)
{
	auto& transform = ROSE_GETSYSTEM(TransformSystem);
	auto& parentTrx = registry.get<TransformComponent>(parent->element);
	for (auto child : parent->children) {
		auto entity = child->element;
		auto& trx = registry.get<TransformComponent>(entity);
		trx.CalcMatrix();
		trx.level = parentTrx.level + 1;
		UpdateChildrenRecursive(registry, child);
	}
}
void LevelTree::TransformDestroyed(entt::registry& registry, entt::entity entity)
{
	if (nodesMap.find(entity) == nodesMap.end()) {
		ROSE_ERR("trying to delete entity that doesnt exist in levelTree");
		return;
	}
	auto node = nodesMap[entity];
	auto& entities = ROSE_GETSYSTEM(Entities);
	for (auto child : node->children) {
		entities.DestroyEntity(child->element);
	}
	node->children.clear();
	nodesMap.erase(entity);
}
void LevelTree::RemoveParent(entt::entity entity)
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	auto& transformSystem = ROSE_GETSYSTEM(TransformSystem);
	auto& childTrx = registry.get<TransformComponent>(entity);
	if (childTrx.hasParent) {
		auto parent = childTrx.parent;
		auto& parentTrx = registry.get<TransformComponent>(parent);
		childTrx.hasParent = false;
		TransformSystem::MoveTransformToWorldSpace(childTrx);
		root->AddChild(nodesMap[entity]);
		ROSE_LOG("removing parent");
	}
}
bool LevelTree::TrySetParent(entt::entity child, entt::entity parent)
{
	if (IsChildOf(child, parent)) {
		return false;
	}
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	auto& transformSystem = ROSE_GETSYSTEM(TransformSystem);
	auto& childTrx = registry.get<TransformComponent>(child);
	auto& parentTrx = registry.get<TransformComponent>(parent);
	childTrx.hasParent = true;
	childTrx.parent = parent;
	auto node = nodesMap[child];
	nodesMap[parent]->AddChild(node);
	TransformSystem::MoveTransformToParentSpace(childTrx, parentTrx);
	ROSE_LOG("setting parent");
	return true;
}
bool LevelTree::IsChildOf(entt::entity entity, entt::entity child) {
	auto node = nodesMap[entity];
	auto childNode = nodesMap[child];

	if (node->children.find(childNode) != node->children.end()) {
		return true;
	}
	else {
		for (auto childNode : node->children) {
			if (IsChildOf(childNode->element, child)) {
				return true;
			}
		}
	}
	return false;
}
void LevelTree::Clear() {
	root->DeleteChildren();
	nodesMap.clear();
}
Node<entt::entity>* LevelTree::GetRoot()
{
	return root;
}
void LevelTree::CleanTree()
{
	auto& entities = ROSE_GETSYSTEM(Entities);
	for (auto& node : nodesMap) {
		std::vector<Node<entt::entity>*> remove;
		for (auto child : node.second->children) {
			if (!entities.EntityExists(child->element)) {
				remove.push_back(child);
			}
		}
		for (auto child : remove) {
			node.second->children.erase(child);
		}
	}
}
entt::entity LevelTree::GetChild(entt::entity entity, const std::string& name)
{
	auto& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	if (!registry.valid(entity)) {
		return NoEntity();
	}
	if (nodesMap.find(entity) == nodesMap.end()) {
		return NoEntity();
	}
	auto node = nodesMap[entity];
	for (auto child : node->children) {
		if (!registry.valid(child->element)) {
			continue;
		}
		if (registry.get<GUIDComponent>(child->element).name == name) {
			return child->element;
		}
	}
	return NoEntity();
}
Node<entt::entity>* LevelTree::AddEntity(entt::entity entity, Node<entt::entity>* parent) {
	if (parent == nullptr) {
		parent = root;
	}
	auto node = new Node<entt::entity>(entity, parent);
	nodesMap[entity] = node;
	return node;
}