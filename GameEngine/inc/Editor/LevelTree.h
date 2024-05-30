#pragma once
#include <map>
#include <set>

#include <entt/entt.hpp>
#include <imgui.h>

#include "Structures/Tree.h"

#include "Entity.h"

#include "Components/GUIDComponent.h"
#include "Components/TransformComponent.h"

#include "Systems.h"

#include "Editor/TransformEditor.h"


namespace LevelEditor {

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
		Node<entt::entity>* root;
		std::unordered_map<entt::entity, Node<entt::entity>*> nodesMap;
		ChildCommand currentCommand;

		Node<entt::entity>* InitParentRecursive(entt::registry& registry, entt::entity entity) {
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
		void EditorChildren(entt::registry& registry, entt::entity& selected, bool* entityList, Node<entt::entity>* node) {
			const auto& guid = registry.get<GUIDComponent>(node->element);
			const auto& trx = registry.get<TransformComponent>(node->element);
			if (selected != node->element) {
				entityList[(int)node->element] = false;
			}
			else {
				entityList[(int)node->element] = true;
			}
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (entityList[(int)node->element] ? ImGuiTreeNodeFlags_Selected : 0)
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
				selected = node->element;
			}
			if (open)
			{
				for (auto node : node->children) {
					EditorChildren(registry, selected, entityList, node);
				}
				ImGui::TreePop();
			}
		}

		void ShowEntity(entt::registry& registry, entt::entity& selected) {
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
				selected = entt::entity(-1);
			}
		}

	public:

		Node<entt::entity>* AddEntity(entt::entity entity) {
			auto node = new Node<entt::entity>(entity, root);
			root->children.insert(node);
			nodesMap[entity] = node;
			return node;
		}
		LevelTree() {
			root = new Node<entt::entity>(NoEntity());
		}
		~LevelTree() {
			delete root;
		}

		void Clear() {
			root->DeleteChildren();
			nodesMap.clear();
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
			ShowEntity(registry, selected);
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