#pragma once
#include <SDL2/SDL.h>

#include "Editor/LevelTree.h"

enum Tools {
	//Entity Tools
	CreateEntity,
	MoveEntity,
	DeleteEntity,
};

class Editor {
	SDL_Window* window;
	SDL_Renderer* renderer;
	LevelEditor::LevelTree levelTree;
	entt::entity selectedEntity;
	entt::entity createdEntity;
	bool mouseInViewport;
	Tools selectedTool;
	void SetupImgui();
	void CloseImgui();
	void PresentImGui();
	void RenderImgui();
	void RenderTools();
	void UpdateViewportControls();
public:
	Editor();
	~Editor();
	void Reset();
	void Update();
	bool ProcessEvents();
	void RenderGizmos();
	void RenderEditor();
};