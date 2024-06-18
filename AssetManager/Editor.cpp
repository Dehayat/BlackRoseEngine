#include "Editor.h"

#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>h

#include <imgui_impl_sdl2.h>

#include "SdlContainer.h"

#include "Systems.h"

#include "AssetManager.h"

Editor::Editor()
{
	CREATESYSTEM(AssetManager);
	SetupImgui();
	isGameRunning = false;
}

void Editor::SetupImgui()
{
	auto& sdl = GETSYSTEM(SdlContainer);
	SDL_RenderSetVSync(sdl.GetRenderer(), 1);

	window = sdl.GetWindow();
	renderer = sdl.GetRenderer();
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	IMGUI_CHECKVERSION();
	auto imguiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(imguiContext);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
}

Editor::~Editor()
{
	CloseImgui();
}

void Editor::CloseImgui()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}


bool Editor::ProcessEvents()
{
	bool exit = false;
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			exit = true;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				exit = true;
			}
			break;
		default:
			break;
		}
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
	}
	return exit;
}


void Editor::RenderEditor()
{
	RenderImgui();

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	auto& assetManager = GETSYSTEM(AssetManager);
	ImGui::Begin("Asset Manager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(w / 2, h));
	ImGui::SetWindowPos(ImVec2(0, 0));
	assetManager.Render();
	ImGui::End();

	if (assetManager.IsAssetSelected()) {
		ImGui::Begin("Asset Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowSize(ImVec2(w / 2, h));
		ImGui::SetWindowPos(ImVec2(w / 2, 0));
		assetManager.RenderSelectedAsset();
		ImGui::End();
	}

	PresentImGui();
	SDL_RenderPresent(renderer);
}

bool Editor::IsGameRunning()
{
	return isGameRunning;
}

void Editor::PresentImGui()
{
	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void Editor::RenderImgui()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	ImGuiIO& io = ImGui::GetIO();
	mouseInViewport = !io.WantCaptureMouse;
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}
