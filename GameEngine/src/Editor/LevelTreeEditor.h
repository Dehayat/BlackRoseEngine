#pragma once
#include "Core/LevelTree.h"

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

class LevelTreeEditor {
	ChildCommand currentCommand;
	entt::entity selectedEntity;
	void ShowEntity(entt::registry& registry);
	void EditorChildren(entt::registry& registry, Node<entt::entity>* node);
public:
	LevelTreeEditor();
	void Editor();
	entt::entity GetSelectedEntity();
	void SelectEntity(entt::entity entity);
	void CleanTree();
};