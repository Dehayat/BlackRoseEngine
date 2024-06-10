#pragma once
#include <SDL2/SDL.h>

#include "Components/Components.h"

#include "Editor/LevelTree.h"
#include "Editor/LevelTreeEditor.h"
#include "Editor/ComponentEditor.h"

enum Tools {
	//Entity Tools
	SelectEntity,
	CreateEntity,
	MoveEntity,
	DeleteEntity,
};

class Editor {
	SDL_Window* window;
	SDL_Renderer* renderer;
	LevelTreeEditor levelTreeEditor;
	entt::entity createdEntity;
	bool mouseInViewport;
	Tools selectedTool;
	bool isGameRunning;

	void SetupImgui();
	void CloseImgui();
	void PresentImGui();
	void RenderImgui();
	void RenderTools();
	void EntityEditor();
	void UpdateViewportControls();
public:
	Editor();
	~Editor();
	void Reset();
	void Update();
	bool ProcessEvents();
	void RenderGizmos();
	void RenderEditor();
	entt::entity GetSelectedEntity();
	bool IsGameRunning();


	template<typename TComponent, typename TEditor>
	void RenderComponent(bool removable, const std::string& componentName, entt::entity entity)
	{
		auto& entities = GETSYSTEM(Entities);
		auto& registry = entities.GetRegistry();
		if (registry.any_of<TComponent>(entity)) {
			if (removable && ImGui::Button(("Remove " + componentName).c_str())) {
				registry.remove<TComponent>(entity);
				return;
			}
			TEditor editor = TEditor();
			IComponentEditor* compEditor = static_cast<IComponentEditor*>(&editor);
			if (ImGui::CollapsingHeader(componentName.c_str())) {
				compEditor->Editor(entity);
			}
		}
		else {
			if (ImGui::Button(("Add " + componentName).c_str())) {
				registry.emplace<TComponent>(entity);
			}
		}
	}
};