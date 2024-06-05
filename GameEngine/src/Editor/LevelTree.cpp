#include "Editor/LevelTree.h"

#include "Transform.h"

LevelTree::LevelTree() {
	root = new Node<entt::entity>(NoEntity());
	nodesMap[entt::entity(-1)] = root;
}
LevelTree::~LevelTree() {
	delete root;
}
void LevelTree::TransformCreated(entt::registry& registry, entt::entity entity)
{
	if (nodesMap.find(entity) != nodesMap.end()) {
		Logger::Error("trying to add entity that already exists in levelTree");
		return;
	}
	auto parent = root;
	auto& trx = registry.get<TransformComponent>(entity);
	trx.level = 0;
	bool isWaitingForParent = false;
	if (trx.hasParent) {
		if (!trx.parent || nodesMap.find(trx.parent.value()) == nodesMap.end()) {
			Logger::Log("parent of new entity not found in levelTree");
			parent = root;
			isWaitingForParent = true;
		}
		else {
			parent = nodesMap[trx.parent.value()];
			auto& parentTrx = registry.get<TransformComponent>(trx.parent.value());
			trx.level = parentTrx.level + 1;
		}
	}
	auto node = AddEntity(entity, parent);

	if (isWaitingForParent) {
		waitingForParent[trx.parentGUID].push_back(node);
	}

	auto& transformSystem = GETSYSTEM(TransformSystem);
	trx.matrixL2W = transformSystem.CalcMatrixL2W(trx);

	auto guid = GETSYSTEM(Entities).GetEntityGuid(entity);
	if (waitingForParent.find(guid) != waitingForParent.end()) {
		for (auto child : waitingForParent[guid]) {
			node->AddChild(child);
			auto& childTrx = registry.get<TransformComponent>(child->element);
			childTrx.parent = node->element;
			childTrx.hasParent = true;
		}
		UpdateChildrenRecursive(registry, node);
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
	if (childTrx.hasParent && childTrx.parent) {
		auto parent = childTrx.parent.value();
		auto& parentTrx = registry.get<TransformComponent>(parent);
		TransformSystem::BakeTransform(childTrx);
		childTrx.hasParent = false;
		root->AddChild(nodesMap[entity]);
		Logger::Log("removing parent");
	}
}
void LevelTree::SetParent(entt::entity child, entt::entity parent)
{
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
}
void LevelTree::UpdateChildrenRecursive(entt::registry& registry, Node<entt::entity>* parent)
{
	auto& transform = GETSYSTEM(TransformSystem);
	auto& parentTrx = registry.get<TransformComponent>(parent->element);
	for (auto child : parent->children) {
		auto entity = child->element;
		auto& trx = registry.get<TransformComponent>(entity);
		trx.matrixL2W = transform.CalcMatrixL2W(trx);
		trx.level = parentTrx.level + 1;
		UpdateChildrenRecursive(registry, child);
	}
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
			if (entities.EntityExists(child->element)) {
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