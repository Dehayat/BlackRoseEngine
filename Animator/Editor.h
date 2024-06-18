#pragma once
#include <SDL2/SDL.h>


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
	bool mouseInViewport;
	Tools selectedTool;
	bool isGameRunning;

	void SetupImgui();
	void CloseImgui();
	void PresentImGui();
	void RenderImgui();
public:
	Editor();
	~Editor();
	bool ProcessEvents();
	void RenderEditor();
	bool IsGameRunning();
};