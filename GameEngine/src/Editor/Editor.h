#pragma once
#include <SDL2/SDL.h>
#include <imgui.h>

#include "Core/BaseGame.h"

#include "Core/Systems.h"

#include "Editor/LevelTreeEditor.h"
#include "Editor/ComponentEditor.h"
#include "MoveTool.h"

enum class Tools
{
	//Entity Tools
	SelectEntity,
	CreateEntity,
	MoveEntity,
	DeleteEntity,
	NoTool,
};

namespace Gizmos
{
	enum
	{
		NONE = 0,
		TRANSFORM,
		PHYSICS,
		ALL
	};
}

class Editor:public BaseGame
{
	bool isRunning;
	LevelTreeEditor levelTreeEditor;
	entt::entity createdEntity;
	bool mouseInViewport;
	Tools selectedTool;
	int gizmosSetting;
	bool isGameRunning;
	bool renameEntity;
	MoveTool moveTool;

	void SetupImgui();
	void CloseImgui();
	void PresentImGui();
	void RenderImgui();
	void RenderTools();
	void RenderToolButton(std::string name, Tools tool);
	void EntityEditor();
	void UpdateViewportControls();
	void UpdateGlobalControls();
	void UpdateSelectTool();
public:
	Editor();
	~Editor();
	virtual void Run() override;
	void Reset();
	void Update();
	void Render();
	bool ProcessEvents();
	void HandleDropFile(SDL_Event& sdlEvent);
	void RenderGizmos();
	void RenderEditor();
	entt::entity GetSelectedEntity();
	bool IsGameRunning();
	int GetGizmos();
	void RenderEntityEditor(entt::entity entity);


	template<typename TComponent, typename TEditor>
	void RenderComponent(bool removable, const std::string& componentName, entt::entity entity)
	{
		auto& entities = ROSE_GETSYSTEM(EntitySystem);
		auto& registry = entities.GetRegistry();

		if(registry.any_of<TComponent>(entity))
		{
			TEditor editor = TEditor();
			IComponentEditor* compEditor = (IComponentEditor*)&editor;
			bool removeComponent = false;
			if(ImGui::CollapsingHeader(componentName.c_str()))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.2, 0.2, 1));
				auto contentSize = ImGui::GetContentRegionAvail();
				if(removable && ImGui::Button(("Remove Component##" + componentName).c_str(), {contentSize.x,0}))
				{
					removeComponent = true;
				}
				ImGui::Separator();
				ImGui::PopStyleColor();
				compEditor->Editor(entity);
			}
			if(removeComponent)
			{
				registry.remove<TComponent>(entity);
			}
		} else
		{
			if(ImGui::Button(("Add " + componentName).c_str()))
			{
				registry.emplace<TComponent>(entity);
			}
		}
	}
};