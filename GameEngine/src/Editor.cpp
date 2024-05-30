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
	selectedEntity = entt::entity(-1);
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
			selectedEntity = createdEntity;
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
		if (selectedEntity != entt::entity(-1)) {
			if (input.GetMouseButton(LEFT_BUTTON).isPressed) {
				registry.get<TransformComponent>(selectedEntity).position = glm::vec2(mousePos.x, mousePos.y);
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
bool entityList[100];

void Editor::RenderEditor()
{
	RenderImgui();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin("ToolBar", nullptr, windowFlags);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	ImGui::SetWindowSize(ImVec2(w, 90));
	ImGui::SetWindowPos(ImVec2(0, 0));

	RenderTools();
	levelTree.Editor(selectedEntity, entityList);

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
		selectedTool = Tools::DeleteEntity;
	}

	ImGui::TableNextColumn();
	auto view = registry.view<const GUIDComponent, TransformComponent>();
	{
		ImGui::BeginTable("Load", 2);
		ImGui::TableNextColumn();
		static char fileName[20] = "Level.yaml";
		if (ImGui::Button("Load Level")) {
			levelLoader.UnloadLevel();
			levelLoader.LoadLevel(fileName);
			levelTree.Init();
		}
		ImGui::TableNextColumn();
		ImGui::InputText(" ", fileName, 20);
		ImGui::EndTable();
	}
	{
		if (ImGui::Button("Save Level")) {
			levelLoader.SaveLevel("SavedLevel.yaml");
		}
	}
	{
		ImGui::BeginTable("Save", 2);
		static char fileName[20] = "NewLevel.yaml";
		ImGui::TableNextColumn();
		if (ImGui::Button("Save Level As")) {
			levelLoader.SaveLevel(fileName);
		}
		ImGui::TableNextColumn();
		ImGui::InputText(" ", fileName, 20);
		ImGui::EndTable();
	}
	ImGui::EndTable();

	if (selectedEntity != entt::entity(-1)) {
		if (registry.any_of<TransformComponent>(selectedEntity)) {
			ImGui::BeginChild("Transform component");
			TransformEditor::DrawEditor(registry.get<TransformComponent>(selectedEntity));
			ImGui::EndChild();
		}
		if (registry.any_of<PhysicsBodyComponent>(selectedEntity)) {
			ImGui::BeginChild("PhysicsBody component");
			auto trx = registry.get<TransformComponent>(selectedEntity);
			PhysicsEditor::DrawEditor(registry.get<PhysicsBodyComponent>(selectedEntity), trx);
			ImGui::EndChild();
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
			ImGui::BeginChild("Sprite component");
			SpriteEditor::DrawEditor(registry.get<SpriteComponent>(selectedEntity));
			ImGui::EndChild();
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
			ImGui::BeginChild("Camera component");
			CameraEditor::DrawEditor(registry.get<CameraComponent>(selectedEntity));
			ImGui::EndChild();
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
}

void Editor::PresentImGui()
{
	ImGui::Render();
	//SDL_SetRenderTarget(renderer, nullptr);
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	//SDL_RenderPresent(renderer);
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
