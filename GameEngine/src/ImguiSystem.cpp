#ifdef _EDITOR
#include "ImguiSystem.h"
#include <string>
#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_impl_sdl2.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
using namespace glm;

ImguiSystem::ImguiSystem()
{
	window = SDL_CreateWindow("Imgui Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 200, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	IMGUI_CHECKVERSION();
	auto imguiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(imguiContext);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
}

ImguiSystem::~ImguiSystem()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	if (SDL_WasInit(0) != 0) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
}


void ImguiSystem::Render()
{
	ImGuiIO& io = ImGui::GetIO();
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}
void ImguiSystem::Present() {
	ImGui::Render();
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
}

void ImguiSystem::HandleEvent(SDL_Event& e)
{
	ImGui_ImplSDL2_ProcessEvent(&e);
}

bool ImguiSystem::ProcessEvents()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		if (SDL_GetKeyboardFocus() != window) {
			return false;
		}
		if (sdlEvent.type == SDL_QUIT) {
			return true;
		}
		if (sdlEvent.type == SDL_WINDOWEVENT) {
			if (sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
				return true;
			}
		}
		HandleEvent(sdlEvent);
	}
	return false;
}
#endif // _EDITOR