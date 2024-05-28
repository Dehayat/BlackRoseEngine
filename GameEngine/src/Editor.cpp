#include "Editor/Editor.h"

#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_impl_sdl2.h>
#include <glm/glm.hpp>

#include "SdlContainer.h"

#include "Systems.h"

#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"


#include "Editor/PhysicsEditor.h"
#include "Editor/RenderEditor.h"
#include "Editor/TransformEditor.h"

Editor::Editor()
{
	SetupImgui();
	Reset();
}

void Editor::SetupImgui()
{
	auto& sdl = GETSYSTEM(SdlContainer);
	SDL_RenderSetVSync(sdl.GetRenderer(), 1);

	window = SDL_CreateWindow("Imgui Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
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

void Editor::Reset()
{
	levelTree.Init();
}

void Editor::CloseImgui()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	if (SDL_WasInit(0) != 0) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
}

void Editor::Update()
{
	SDL_Event sdlEvent;
	if (SDL_GetKeyboardFocus() != window) {
		return;
	}
	while (SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
	}
}

void Editor::RenderGizmos()
{
}
bool entityList[100];

void Editor::RenderEditor()
{
	RenderImgui();

	//ImGui::ShowDemoWindow(nullptr);
	auto& registry = GETSYSTEM(Entities).GetRegistry();
	if (selectedEntity != entt::entity(-1)) {
		if (registry.any_of<TransformComponent>(selectedEntity)) {
			ImGui::Begin("Transform component");
			TransformEditor::DrawEditor(registry.get<TransformComponent>(selectedEntity));
			ImGui::End();
		}
		if (registry.any_of<PhysicsBodyComponent>(selectedEntity)) {
			ImGui::Begin("PhysicsBody component");
			auto trx = registry.get<TransformComponent>(selectedEntity);
			PhysicsEditor::DrawEditor(registry.get<PhysicsBodyComponent>(selectedEntity), trx);
			ImGui::End();
			if (ImGui::Button("Remove PhysicsBody Component")) {
				registry.remove<PhysicsBodyComponent>(selectedEntity);
			}
		}
		else {
			if (ImGui::Button("Add PhysicsBody Component")) {
				registry.emplace<PhysicsBodyComponent>(selectedEntity);
			}
		}
		if (registry.any_of<SpriteComponent>(selectedEntity)) {
			ImGui::Begin("Sprite component");
			SpriteEditor::DrawEditor(registry.get<SpriteComponent>(selectedEntity));
			ImGui::End();
			if (ImGui::Button("Remove Sprite Component")) {
				registry.remove<SpriteComponent>(selectedEntity);
			}
		}
		else {
			if (ImGui::Button("Add Sprite Component")) {
				registry.emplace<SpriteComponent>(selectedEntity, "block");
			}
		}
		if (registry.any_of<CameraComponent>(selectedEntity)) {
			ImGui::Begin("Camera component");
			CameraEditor::DrawEditor(registry.get<CameraComponent>(selectedEntity));
			ImGui::End();
			if (ImGui::Button("Remove Camera Component")) {
				registry.remove<CameraComponent>(selectedEntity);
			}
		}
		else {
			if (ImGui::Button("Add Camera Component")) {
				registry.emplace<CameraComponent>(selectedEntity);
			}
		}
	}

	levelTree.Editor(selectedEntity,entityList);

	PresentImGui();
}

void Editor::PresentImGui()
{
	ImGui::Render();
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
}

void Editor::RenderImgui()
{
	ImGuiIO& io = ImGui::GetIO();
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}
