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
		entt::entity selectedEntity;

		Node<entt::entity>* InitParentRecursive(entt::registry& registry, entt::entity entity);
		void EditorChildren(entt::registry& registry, Node<entt::entity>* node);
		void ShowEntity(entt::registry& registry);
	
	public:
		Node<entt::entity>* AddEntity(entt::entity entity);
		LevelTree();
		~LevelTree();
		void Clear();
		void Init();
		void Editor();
		entt::entity GetSelectedEntity();
		void SelectEntity(entt::entity entity);
	};
}