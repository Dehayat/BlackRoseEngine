#include "Editor/Editor.h"

#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>h

#include <imgui_impl_sdl2.h>
#include <glm/glm.hpp>

#include "SdlContainer.h"
#include "LevelLoader.h"

#include "InputSystem.h"
#include "Renderer.h"

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

	//window = SDL_CreateWindow("Imgui Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
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

void Editor::Reset()
{
	createdEntity = entt::entity(-1);
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
	if (mouseInViewport) {
		UpdateViewportControls();
	}
}

void Editor::UpdateViewportControls()
{
	auto& entities = GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	auto& input = GETSYSTEM(InputSystem);
	auto& gameRenderer = GETSYSTEM(RendererSystem);
	if (selectedTool == Tools::CreateEntity) {
		auto mousePos = glm::vec3(input.GetMousePosition(), 1) * gameRenderer.GetScreenToWorldMatrix();
		if (input.GetMouseButton(LEFT_BUTTON).justPressed) {
			Logger::Log("entity created");
			createdEntity = entities.CreateEntity();
			levelTree.AddEntity(createdEntity);
			levelTree.SelectEntity(createdEntity);
			registry.emplace<TransformComponent>(createdEntity, glm::vec2(mousePos.x, mousePos.y), glm::vec2(1, 1), 0);
		}
		if (createdEntity != entt::entity(-1)) {
			if (input.GetMouseButton(LEFT_BUTTON).justReleased) {
				createdEntity = entt::entity(-1);
			}
			if (input.GetMouseButton(LEFT_BUTTON).isPressed) {
				registry.get<TransformComponent>(createdEntity).position = glm::vec2(mousePos.x, mousePos.y);
			}
		}
	}
	if (selectedTool == Tools::MoveEntity) {
		auto mousePos = glm::vec3(input.GetMousePosition(), 1) * gameRenderer.GetScreenToWorldMatrix();
		if (levelTree.GetSelectedEntity() != entt::entity(-1)) {
			if (input.GetMouseButton(LEFT_BUTTON).isPressed) {
				registry.get<TransformComponent>(levelTree.GetSelectedEntity()).position = glm::vec2(mousePos.x, mousePos.y);
			}
		}
	}
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

void Editor::RenderGizmos()
{
}

void Editor::RenderEditor()
{
	RenderImgui();

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	ImGui::ShowDemoWindow();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin("ToolBar", nullptr, windowFlags);
	ImGui::SetWindowSize(ImVec2(w, 90));
	ImGui::SetWindowPos(ImVec2(0, 0));
	RenderTools();
	ImGui::End();

	ImGui::Begin("Level Tree", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(ImVec2(200, 200));
	ImGui::SetWindowPos(ImVec2(w - 200, 90));
	levelTree.Editor();
	ImGui::End();

	ImGui::Begin("Entity Properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(ImVec2(200, h - 290));
	ImGui::SetWindowPos(ImVec2(w - 200, 290));
	EntityEditor();
	ImGui::End();



	PresentImGui();
}

void Editor::RenderTools()
{
	auto& entities = GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	auto& input = GETSYSTEM(InputSystem);
	auto& gameRenderer = GETSYSTEM(RendererSystem);
	auto& levelLoader = GETSYSTEM(LevelLoader);
	ImGui::BeginTable("Tools", 3);
	ImGui::TableNextColumn();
	if (ImGui::Button("Create Entity")) {
		selectedTool = Tools::CreateEntity;
	}
	if (ImGui::Button("Move Entity")) {
		selectedTool = Tools::MoveEntity;
	}
	if (ImGui::Button("Delete Entity")) {
		if (levelTree.GetSelectedEntity() != NoEntity()) {
			entities.DestroyEntity(levelTree.GetSelectedEntity());
			levelTree.Init();
		}
	}

	ImGui::TableNextColumn();
	auto view = registry.view<const GUIDComponent, TransformComponent>();
	{
		static char fileName[20] = "Level.yaml";
		if (ImGui::Button("Load Level")) {
			levelLoader.UnloadLevel();
			levelLoader.LoadLevel(fileName);
			levelTree.Init();
		}
		ImGui::SameLine();
		ImGui::InputText(" ", fileName, 20);
	}
	{
		if (ImGui::Button("Save Level")) {
			levelLoader.SaveLevel("SavedLevel.yaml");
		}
	}
	{
		static char fileName[20] = "NewLevel.yaml";
		if (ImGui::Button("Save Level As")) {
			levelLoader.SaveLevel(fileName);
		}
		ImGui::SameLine();
		ImGui::InputText(" ", fileName, 20);
	}
	ImGui::EndTable();
}

void Editor::EntityEditor()
{
	auto& entities = GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	auto selectedEntity = levelTree.GetSelectedEntity();
	if (levelTree.GetSelectedEntity() != entt::entity(-1)) {
		if (registry.any_of<TransformComponent>(selectedEntity)) {
			if (ImGui::CollapsingHeader("Transform component")) {
				TransformEditor::DrawEditor(registry.get<TransformComponent>(selectedEntity));
			}
		}
		if (registry.any_of<PhysicsBodyComponent>(selectedEntity)) {
			if (ImGui::CollapsingHeader("Transform component")) {
				auto& trx = registry.get<TransformComponent>(selectedEntity);
				PhysicsEditor::DrawEditor(registry.get<PhysicsBodyComponent>(selectedEntity), trx);
				if (ImGui::Button("Remove PhysicsBody Component")) {
					registry.remove<PhysicsBodyComponent>(selectedEntity);
				}
			}
		}
		else {
			if (ImGui::Button("Add PhysicsBody Component")) {
				registry.emplace<PhysicsBodyComponent>(selectedEntity);
			}
		}
		if (registry.any_of<SpriteComponent>(selectedEntity)) {
			if (ImGui::CollapsingHeader("Sprite component")) {
				SpriteEditor::DrawEditor(registry.get<SpriteComponent>(selectedEntity));
				if (ImGui::Button("Remove Sprite Component")) {
					registry.remove<SpriteComponent>(selectedEntity);
				}
			}
		}
		else {
			if (ImGui::Button("Add Sprite Component")) {
				registry.emplace<SpriteComponent>(selectedEntity, "block");
			}
		}
		if (registry.any_of<CameraComponent>(selectedEntity)) {
			if (ImGui::CollapsingHeader("Camera Component")) {
				CameraEditor::DrawEditor(registry.get<CameraComponent>(selectedEntity));
				if (ImGui::Button("Remove Camera Component")) {
					registry.remove<CameraComponent>(selectedEntity);
				}
			}
		}
		else {
			if (ImGui::Button("Add Camera Component")) {
				registry.emplace<CameraComponent>(selectedEntity);
			}
		}
	}
}

void Editor::PresentImGui()
{
	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void Editor::RenderImgui()
{
	ImGuiIO& io = ImGui::GetIO();
	mouseInViewport = !io.WantCaptureMouse;
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}
