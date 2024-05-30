#include "Editor/LevelTree.h"


namespace LevelEditor {
	Node<entt::entity>* LevelTree::InitParentRecursive(entt::registry& registry, entt::entity entity) {
		if (nodesMap.find(entity) == nodesMap.end()) {
			auto& trx = registry.get<TransformComponent>(entity);
			if (trx.parent) {
				auto node = new Node<entt::entity>(entity);
				auto parentNode = InitParentRecursive(registry, trx.parent.value());
				node->SetParent(parentNode);
				nodesMap[entity] = node;
			}
			else {
				AddEntity(entity);
			}
		}
		return nodesMap[entity];
	}
	void LevelTree::EditorChildren(entt::registry& registry, Node<entt::entity>* node) {
		const auto& guid = registry.get<GUIDComponent>(node->element);
		const auto& trx = registry.get<TransformComponent>(node->element);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| (node->element == selectedEntity ? ImGuiTreeNodeFlags_Selected : 0)
			| (node->children.size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0);
		auto open = ImGui::TreeNodeEx(std::to_string(guid.id).c_str(), flags);
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Node<entt::entity>", &(node->element), sizeof(node->element));
			ImGui::Text(std::to_string(guid.id).c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Node<entt::entity>"))
			{
				auto entity = (entt::entity*)(payload->Data);
				currentCommand.child = *entity;
				currentCommand.newParent = node->element;
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked()) {
			selectedEntity = node->element;
		}
		if (open)
		{
			for (auto node : node->children) {
				EditorChildren(registry, node);
			}
			ImGui::TreePop();
		}
	}
	entt::entity LevelTree::GetSelectedEntity()
	{
		return selectedEntity;
	}
	void LevelTree::SelectEntity(entt::entity entity)
	{
		selectedEntity = entity;
	}
	LevelTree::LevelTree() {
		selectedEntity = NoEntity();
		root = new Node<entt::entity>(NoEntity());
	}
	LevelTree::~LevelTree() {
		delete root;
	}
	void LevelTree::Clear() {
		root->DeleteChildren();
		nodesMap.clear();
	}
	void LevelTree::ShowEntity(entt::registry& registry) {
		ImGui::Text("Level");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Node<entt::entity>"))
			{
				auto entity = (entt::entity*)(payload->Data);
				currentCommand.child = *entity;
				currentCommand.newParent = entt::entity(-1);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked()) {
			selectedEntity = entt::entity(-1);
		}
	}
	Node<entt::entity>* LevelTree::AddEntity(entt::entity entity) {
		auto node = new Node<entt::entity>(entity, root);
		root->children.insert(node);
		nodesMap[entity] = node;
		return node;
	}
	void LevelTree::Init() {
		selectedEntity = NoEntity();
		root->DeleteChildren();
		nodesMap.clear();
		nodesMap[entt::entity(-1)] = root;

		auto& registry = GETSYSTEM(Entities).GetRegistry();
		auto view = registry.view<const TransformComponent>();
		for (auto entity : view) {
			if (nodesMap.find(entity) != nodesMap.end()) {
				continue;
			}
			const auto& trx = view.get<TransformComponent>(entity);
			if (trx.level == 0) {
				AddEntity(entity);
			}
			else {
				InitParentRecursive(registry, entity);
			}
		}
	}
	void LevelTree::Editor()
	{
		auto& registry = GETSYSTEM(Entities).GetRegistry();
		ShowEntity(registry);
		for (auto node : root->children) {
			EditorChildren(registry, node);
		}
		if (currentCommand.child != entt::entity(-1)) {
			nodesMap[currentCommand.child]->SetParent(nodesMap[currentCommand.newParent]);
			TransformEditor::SetParent(registry, registry.get<TransformComponent>(currentCommand.child), currentCommand.newParent);
			currentCommand.Reset();
		}
	}
}