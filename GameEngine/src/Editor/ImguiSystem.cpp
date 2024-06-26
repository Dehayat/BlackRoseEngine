#include "Editor/ImguiSystem.h"
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_impl_sdl2.h>
#include "Core/Systems.h"
#include "Core/SdlContainer.h"

ImguiSystem::ImguiSystem()
{
	auto& sdl = ROSE_GETSYSTEM(SdlContainer);
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
	capturedMouse = false;
}

ImguiSystem::~ImguiSystem()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}


void ImguiSystem::Render()
{
	ImGuiIO& io = ImGui::GetIO();
	capturedMouse = io.WantCaptureMouse;
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}
void ImguiSystem::Present()
{
	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void ImguiSystem::HandleEvent(SDL_Event& e)
{
	ImGui_ImplSDL2_ProcessEvent(&e);
}

bool ImguiSystem::IsMouseCaptured()
{
	return capturedMouse;
}
