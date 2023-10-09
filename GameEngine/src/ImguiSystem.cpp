#include "ImguiSystem.h"

#include <imgui_impl_sdlrenderer2.h>
#include <imgui_impl_sdl2.h>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
using namespace glm;

ImguiSystem::ImguiSystem()
{
	window = SDL_CreateWindow("Imgui Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 900, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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


void ImguiSystem::Render(glm::mat3 view, glm::mat3 world, glm::mat3 cam, glm::vec2 pos)
{

	ImGuiIO& io = ImGui::GetIO();
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);

	//// ImGui NewFrame and rendering for the second window
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//// ImGui code here
	ImGui::ShowDemoWindow();
	ImGui::SliderFloat("CamX", &x, -10, 10);
	ImGui::SliderFloat("CamY", &y, -10, 10);
	std::string x = "view mat" + to_string(view);
	std::string y = "world mat" + to_string(world);
	std::string z = "cam mat" + to_string(cam);
	std::string a = "cam vec" + to_string(pos);
	ImGui::Text(x.c_str());
	ImGui::Text(y.c_str());
	ImGui::Text(z.c_str());
	ImGui::Text(a.c_str());

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
