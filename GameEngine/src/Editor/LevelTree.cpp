#include "Editor/LevelTree.h"

#include "Transform.h"

#include "Debugging/Logger.h"

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
		Logger::Error("trying to add entity that already exists in levelTree");
		return;
	}
	auto& entities = GETSYSTEM(Entities);
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

	auto& transformSystem = GETSYSTEM(TransformSystem);
	trx.CalcMatrix();

	auto guid = GETSYSTEM(Entities).GetEntityGuid(entity);
	ConnectWaitingChildren(node, guid);
}
Node<entt::entity>* LevelTree::TryGetParent(TransformComponent& trx)
{
	auto& entities = GETSYSTEM(Entities);
	if (entities.EntityExists(trx.parent)) {
		trx.parentGUID = entities.GetEntityGuid(trx.parent);
	}
	if (entities.EntityExists(trx.parentGUID)) {
		trx.parent = entities.GetEntity(trx.parentGUID);
	}
	if (entities.EntityExists(trx.parent)) {
		if (nodesMap.find(trx.parent) != nodesMap.end()) {
			Logger::Log("parent of new entity not found in levelTree");
			return nodesMap[trx.parent];
		}
	}
	return nullptr;
}
void LevelTree::ConnectWaitingChildren(Node<entt::entity>* parentNode, Guid parentGuid)
{
	auto& entities = GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	if (waitingForParent.find(parentGuid) != waitingForParent.end()) {
		Logger::Log("found parent");
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
	auto& transform = GETSYSTEM(TransformSystem);
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
		Logger::Error("trying to delete entity that doesnt exist in levelTree");
		return;
	}
	auto node = nodesMap[entity];
	auto& entities = GETSYSTEM(Entities);
	for (auto child : node->children) {
		entities.DestroyEntity(child->element);
	}
	node->children.clear();
	nodesMap.erase(entity);
}
void LevelTree::RemoveParent(entt::entity entity)
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	auto& transformSystem = GETSYSTEM(TransformSystem);
	auto& childTrx = registry.get<TransformComponent>(entity);
	if (childTrx.hasParent) {
		auto parent = childTrx.parent;
		auto& parentTrx = registry.get<TransformComponent>(parent);
		childTrx.hasParent = false;
		TransformSystem::MoveTransformToWorldSpace(childTrx);
		root->AddChild(nodesMap[entity]);
		Logger::Log("removing parent");
	}
}
bool LevelTree::TrySetParent(entt::entity child, entt::entity parent)
{
	if (IsChildOf(child, parent)) {
		return false;
	}
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	auto& transformSystem = GETSYSTEM(TransformSystem);
	auto& childTrx = registry.get<TransformComponent>(child);
	auto& parentTrx = registry.get<TransformComponent>(parent);
	childTrx.hasParent = true;
	childTrx.parent = parent;
	auto node = nodesMap[child];
	nodesMap[parent]->AddChild(node);
	TransformSystem::MoveTransformToParentSpace(childTrx, parentTrx);
	Logger::Log("setting parent");
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
	auto& entities = GETSYSTEM(Entities);
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
Node<entt::entity>* LevelTree::AddEntity(entt::entity entity, Node<entt::entity>* parent) {
	if (parent == nullptr) {
		parent = root;
	}
	auto node = new Node<entt::entity>(entity, parent);
	nodesMap[entity] = node;
	return node;
}