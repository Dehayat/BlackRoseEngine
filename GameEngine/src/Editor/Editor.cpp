#include "Editor/Editor.h"

#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>h

#include <imgui_impl_sdl2.h>
#include <glm/glm.hpp>

#include "SdlContainer.h"
#include "LevelLoader.h"

#include "Input/InputSystem.h"
#include "Renderer/Renderer.h"

#include "Systems.h"

#include "Components/GUIDComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/ScriptComponent.h"
#include "Components/PlayerComponent.h"


#include "Editor/GuidEditor.h"
#include "Editor/PhysicsEditor.h"
#include "Editor/RenderEditor.h"
#include "Editor/TransformEditor.h"
#include "Editor/AnimationEditor.h"
#include "Editor/ScriptEditor.h"
#include "Editor/PlayerEditor.h"

#include "Tools/AssetManager.h"

Editor::Editor()
{
	CREATESYSTEM(AssetManager);
	SetupImgui();
	Reset();
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

void Editor::Reset()
{
	createdEntity = entt::entity(-1);
}

void Editor::CloseImgui()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
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
			levelTreeEditor.SelectEntity(createdEntity);
			registry.emplace<TransformComponent>(createdEntity, glm::vec2(mousePos.x, mousePos.y), glm::vec2(1, 1), 0);
		}
		if (createdEntity != entt::entity(-1)) {
			if (input.GetMouseButton(LEFT_BUTTON).justReleased) {
				createdEntity = entt::entity(-1);
			}
			if (input.GetMouseButton(LEFT_BUTTON).isPressed) {
				registry.get<TransformComponent>(createdEntity).globalPosition = glm::vec2(mousePos.x, mousePos.y);
				registry.get<TransformComponent>(createdEntity).UpdateLocals();
			}
		}
	}
	if (selectedTool == Tools::MoveEntity) {
		auto mousePos = glm::vec3(input.GetMousePosition(), 1) * gameRenderer.GetScreenToWorldMatrix();
		if (levelTreeEditor.GetSelectedEntity() != entt::entity(-1)) {
			if (input.GetMouseButton(LEFT_BUTTON).isPressed) {
				registry.get<TransformComponent>(levelTreeEditor.GetSelectedEntity()).globalPosition = glm::vec2(mousePos.x, mousePos.y);
				registry.get<TransformComponent>(levelTreeEditor.GetSelectedEntity()).UpdateLocals();
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

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin("ToolBar", nullptr, windowFlags);
	ImGui::SetWindowSize(ImVec2(w, 90));
	ImGui::SetWindowPos(ImVec2(0, 0));
	RenderTools();
	ImGui::End();

	ImGui::Begin("Level Tree", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(ImVec2(200, 200));
	ImGui::SetWindowPos(ImVec2(w - 200, 90));
	levelTreeEditor.Editor();
	ImGui::End();

	ImGui::Begin("Entity Properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(ImVec2(200, h - 290));
	ImGui::SetWindowPos(ImVec2(w - 200, 290));
	EntityEditor();
	ImGui::End();


	auto& assetManager = GETSYSTEM(AssetManager);
	ImGui::Begin("Asset Manager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(ImVec2(400, 300));
	ImGui::SetWindowPos(ImVec2(0, h - 300));
	assetManager.Render();
	ImGui::End();

	if (assetManager.IsAssetSelected()) {
		ImGui::Begin("Asset Editor", nullptr);
		ImGui::SetWindowSize(ImVec2(300, 300));
		ImGui::SetWindowPos(ImVec2(400, h - 300));
		assetManager.RenderSelectedAsset();
		ImGui::End();
	}


	PresentImGui();
}

entt::entity Editor::GetSelectedEntity()
{
	return levelTreeEditor.GetSelectedEntity();
}

bool Editor::IsGameRunning()
{
	return isGameRunning;
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
		if (levelTreeEditor.GetSelectedEntity() != NoEntity()) {
			entities.DestroyEntity(levelTreeEditor.GetSelectedEntity());
			levelTreeEditor.CleanTree();
		}
	}

	ImGui::TableNextColumn();
	auto view = registry.view<const GUIDComponent, TransformComponent>();
	{
		static char fileName[41] = "assets/Levels/Level.yaml";
		if (ImGui::Button("Load Level")) {
			levelLoader.UnloadLevel();
			levelTreeEditor.CleanTree();
			levelLoader.LoadLevel(fileName);
			GETSYSTEM(RendererSystem).InitLoaded();
		}
		ImGui::SameLine();
		ImGui::InputText("##L1", fileName, 41);
	}
	{
		if (ImGui::Button("Save Level")) {
			if (levelLoader.GetCurrentLevelFile() != "") {
				levelLoader.SaveLevel(levelLoader.GetCurrentLevelFile());
			}
		}
	}
	{
		static char fileName2[41] = "assets/Levels/NewLevel.yaml";
		if (ImGui::Button("Save Level As")) {
			levelLoader.SaveLevel(fileName2);
		}
		ImGui::SameLine();
		ImGui::InputText("##L2", fileName2, 41);
	}

	ImGui::TableNextColumn();
	if (IsGameRunning()) {
		if (ImGui::Button("Stop")) {
			isGameRunning = false;
		}
	}
	else {
		if (ImGui::Button("Play")) {
			isGameRunning = true;
		}
	}

	ImGui::EndTable();
}

void Editor::EntityEditor()
{
	auto& entities = GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	auto selectedEntity = levelTreeEditor.GetSelectedEntity();
	if (levelTreeEditor.GetSelectedEntity() != entt::entity(-1)) {
		RenderComponent<GUIDComponent, GuidEditor>(false, "Entity Info", selectedEntity);
		RenderComponent<TransformComponent, TransformEditor>(false, "Transform Component", selectedEntity);
		RenderComponent<PhysicsBodyComponent, PhysicsEditor>(true, "Physics Body Component", selectedEntity);
		RenderComponent<SpriteComponent, SpriteEditor>(true, "Sprite Component", selectedEntity);
		RenderComponent<CameraComponent, CameraEditor>(true, "Camera Component", selectedEntity);
		RenderComponent<AnimationComponent, AnimationEditor>(true, "Animation Component", selectedEntity);
		RenderComponent<ScriptComponent, ScriptEditor>(true, "Script Component", selectedEntity);
		RenderComponent<PlayerComponent, PlayerEditor>(true, "Player Component", selectedEntity);
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
