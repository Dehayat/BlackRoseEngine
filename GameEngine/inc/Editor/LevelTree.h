#pragma once
#include <map>
#include <set>

#include <entt/entt.hpp>
#include <imgui.h>

#include "Entity.h"

#include "Components/GUIDComponent.h"
#include "Components/TransformComponent.h"

#include "Systems.h"

#include "Editor/TransformEditor.h"


namespace LevelEditor {

	struct LevelNode {
		LevelNode* parent;
		entt::entity entity;
		std::unordered_set<LevelNode*> children;
		LevelNode(entt::entity entity = entt::entity(-1), LevelNode* parent = nullptr) {
			this->entity = entity;
			this->parent = parent;
		}
		void SetParent(LevelNode* parent) {
			if (this->parent != nullptr) {
				this->parent->children.erase(this);
			}
			this->parent = parent;
			this->parent->children.insert(this);
		}
		void DeleteChildren() {
			for (auto p : children) {
				delete p;
			}
			children.clear();
		}
		~LevelNode() {
			DeleteChildren();
		}
	};

	struct ChildCommand {
		entt::entity child;
		entt::entity newParent;
		ChildCommand() {
			Reset();
		}
		void Reset() {
			child = entt::entity(-1);
		}
	};

	class LevelTree {
		LevelNode* root;
		std::unordered_map<entt::entity, LevelNode*> nodesMap;
		ChildCommand currentCommand;

		LevelNode* InitParentRecursive(entt::registry& registry, entt::entity entity) {
			if (nodesMap.find(entity) == nodesMap.end()) {
				auto& trx = registry.get<TransformComponent>(entity);
				if (trx.parent) {
					auto node = new LevelNode(entity);
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
		void EditorChildren(entt::registry& registry, entt::entity& selected, bool* entityList, LevelNode* node) {
			const auto& guid = registry.get<GUIDComponent>(node->entity);
			const auto& trx = registry.get<TransformComponent>(node->entity);
			if (selected != node->entity) {
				entityList[(int)node->entity] = false;
			}
			else {
				entityList[(int)node->entity] = true;
			}
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (entityList[(int)node->entity] ? ImGuiTreeNodeFlags_Selected : 0)
				| (node->children.size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0);
			auto open = ImGui::TreeNodeEx(std::to_string(guid.id).c_str(), flags);
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("LevelNode", &(node->entity), sizeof(node->entity));
				ImGui::Text(std::to_string(guid.id).c_str());
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LevelNode"))
				{
					auto entity = (entt::entity*)(payload->Data);
					currentCommand.child = *entity;
					currentCommand.newParent = node->entity;
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemClicked()) {
				selected = node->entity;
			}
			if (open)
			{
				for (auto node : node->children) {
					EditorChildren(registry, selected, entityList, node);
				}
				ImGui::TreePop();
			}
		}

		void ShowLevelNode(entt::registry& registry, entt::entity& selected) {
			ImGui::Text("Level");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LevelNode"))
				{
					auto entity = (entt::entity*)(payload->Data);
					currentCommand.child = *entity;
					currentCommand.newParent = entt::entity(-1);
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemClicked()) {
				selected = entt::entity(-1);
			}
		}

	public:

		LevelNode* AddEntity(entt::entity entity) {
			auto node = new LevelNode(entity, root);
			root->children.insert(node);
			nodesMap[entity] = node;
			return node;
		}
		LevelTree() {
			root = new LevelNode();
		}
		~LevelTree() {
			delete root;
		}

		void Init() {
			root->DeleteChildren();
			nodesMap.clear();
			nodesMap[entt::entity(-1)] = root;

			auto& registry = GETSYSTEM(Entities).GetRegistry();
			auto view = registry.view<const TransformComponent>();
			for (auto entity : view) {
				if (nodesMap.find(entity) != nodesMap.end()) {
					continue;
				}
				const auto trx = view.get<TransformComponent>(entity);
				if (trx.level == 0) {
					AddEntity(entity);
				}
				else {
					InitParentRecursive(registry, entity);
				}
			}
		}

		void Editor(entt::entity& selected, bool* entityList) {

			auto& registry = GETSYSTEM(Entities).GetRegistry();
			ShowLevelNode(registry, selected);
			for (auto node : root->children) {
				EditorChildren(registry, selected, entityList, node);
			}
			if (currentCommand.child != entt::entity(-1)) {
				nodesMap[currentCommand.child]->SetParent(nodesMap[currentCommand.newParent]);
				TransformEditor::SetParent(registry, registry.get<TransformComponent>(currentCommand.child), currentCommand.newParent);
				currentCommand.Reset();
			}
		}
	};

}