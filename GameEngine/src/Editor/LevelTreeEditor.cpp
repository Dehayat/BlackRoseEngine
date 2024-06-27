#include "Editor/LevelTreeEditor.h"

#include <string>

#include <entt/entity/registry.hpp>

#include "Components/GUIDComponent.h"
#include "Components/DisableComponent.h"

#include "Editor/TransformEditor.h"

LevelTreeEditor::LevelTreeEditor()
{
	selectedEntity = NoEntity();
}

void LevelTreeEditor::ShowEntity(entt::registry& registry)
{
	ImGui::Text("Level");
	if(ImGui::BeginDragDropTarget())
	{
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Node<entt::entity>"))
		{
			auto entity = (entt::entity*)(payload->Data);
			currentCommand.child = *entity;
			currentCommand.newParent = NoEntity();
		}
		ImGui::EndDragDropTarget();
	}

	if(ImGui::IsItemClicked())
	{
		selectedEntity = NoEntity();
	}
}

void LevelTreeEditor::EditorChildren(entt::registry& registry, Node<entt::entity>* node)
{
	const auto& guid = registry.get<GUIDComponent>(node->element);
	const auto& trx = registry.get<TransformComponent>(node->element);
	auto disabled = registry.try_get<DisableComponent>(node->element);
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| (node->element == selectedEntity ? ImGuiTreeNodeFlags_Selected : 0)
		| (node->children.size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0);

	bool open = false;

	if(disabled != nullptr)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75, 0.75, 0.75, 0.75));
	}

	if(guid.name == "")
	{
		open = ImGui::TreeNodeEx(std::to_string(guid.id).c_str(), flags);
	} else
	{
		open = ImGui::TreeNodeEx((guid.name + "##" + std::to_string(guid.id)).c_str(), flags);
	}

	if(disabled != nullptr)
	{
		ImGui::PopStyleColor();
	}


	if(ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("Node<entt::entity>", &(node->element), sizeof(node->element));
		ImGui::Text(std::to_string(guid.id).c_str());
		ImGui::EndDragDropSource();
	}
	if(ImGui::BeginDragDropTarget())
	{
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Node<entt::entity>"))
		{
			auto entity = (entt::entity*)(payload->Data);
			currentCommand.child = *entity;
			currentCommand.newParent = node->element;
		}
		ImGui::EndDragDropTarget();
	}

	if(ImGui::IsItemClicked())
	{
		selectedEntity = node->element;
	}
	if(open)
	{
		for(auto node : node->children)
		{
			EditorChildren(registry, node);
		}
		ImGui::TreePop();
	}
}

void LevelTreeEditor::Editor()
{
	auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	ShowEntity(registry);
	auto& levelTree = ROSE_GETSYSTEM(LevelTree);
	for(auto node : levelTree.GetRoot()->children)
	{
		EditorChildren(registry, node);
	}
	if(currentCommand.child != entt::entity(-1))
	{
		levelTree.TrySetParent(currentCommand.child, currentCommand.newParent);
		currentCommand.Reset();
	}
}

entt::entity LevelTreeEditor::GetSelectedEntity()
{
	return selectedEntity;
}

void LevelTreeEditor::SelectEntity(entt::entity entity)
{
	selectedEntity = entity;
}