#include "Editor.h"

#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>

#include <imgui_impl_sdl2.h>

#include "Core/SdlContainer.h"
#include "Levels/LevelLoader.h"
#include "AssetPipline/AssetStore.h"

#include "Input/InputSystem.h"
#include "Renderer/Renderer.h"

#include "Core/Systems.h"

#include "Components/GUIDComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/ScriptComponent.h"

#include "Editor/PhysicsEditor.h"
#include "Editor/ScriptEditor.h"

#include "DefaultEditor.h"

#define ROSE_DEFAULT_COMP_EDITOR(COMP,DEL)	ROSE_INIT_VARS(COMP);\
											RenderComponent<COMP, DefaultComponentEditor<COMP>>(DEL, #COMP, selectedEntity)

Editor::Editor()
{
	SetupImgui();
	Reset();
	isGameRunning = false;
	selectedTool = Tools::SelectEntity;
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
	if (!ImGui::IsAnyItemActive()) {
		UpdateGlobalControls();
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
	if (selectedTool == Tools::SelectEntity) {
		UpdateSelectTool();
	}
}

void Editor::UpdateGlobalControls()
{
	auto& entities = GETSYSTEM(Entities);
	auto& input = GETSYSTEM(InputSystem);
	auto& levelLoader = GETSYSTEM(LevelLoader);
	if (GetSelectedEntity() != NoEntity()) {
		if (input.GetKey(InputKey::DELETE).justReleased) {
			if (levelTreeEditor.GetSelectedEntity() != NoEntity()) {
				entities.DestroyEntity(levelTreeEditor.GetSelectedEntity());
				levelTreeEditor.CleanTree();
			}
		}
	}
	if (input.GetKey(InputKey::LCTRL).isPressed || input.GetKey(InputKey::RCTRL).isPressed) {
		if (input.GetKey(InputKey::N).justPressed) {
			levelLoader.UnloadLevel();
			levelTreeEditor.CleanTree();
		}
	}

	if (input.GetKey(InputKey::C).justPressed) {
		selectedTool = Tools::CreateEntity;
	}
	if (input.GetKey(InputKey::M).justPressed) {
		selectedTool = Tools::MoveEntity;
	}
	if (input.GetKey(InputKey::S).justPressed) {
		selectedTool = Tools::SelectEntity;
	}
	if (input.GetKey(InputKey::X).justPressed) {
		selectedTool = Tools::NoTool;
	}
}

static bool IsPointInsideRect(vec2 point, SDL_FRect rect) {
	if (point.x<rect.x || point.x > rect.x + rect.w) {
		return false;
	}
	if (point.y<rect.y || point.y > rect.y + rect.h) {
		return false;
	}
	return true;
}

void Editor::UpdateSelectTool()
{
	auto& entities = GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	auto& input = GETSYSTEM(InputSystem);
	if (input.GetMouseButton(LEFT_BUTTON).justPressed) {
		auto mousePos = input.GetMousePosition();
		auto view = registry.view<const SpriteComponent>();
		bool selected = false;
		for (auto entity : view) {
			auto& sprite = registry.get<SpriteComponent>(entity);
			if (IsPointInsideRect(mousePos, sprite.destRect)) {
				levelTreeEditor.SelectEntity(entity);
				selected = true;
			}
		}
		if (!selected) {
			levelTreeEditor.SelectEntity(NoEntity());
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
	ImGui::BeginTable("ToolBar", 3, ImGuiTableFlags_BordersInnerV);
	ImGui::TableNextColumn();
	ImGui::BeginTable("Tools", 3, ImGuiTableFlags_BordersInnerV);
	ImGui::TableNextColumn();
	RenderToolButton("Move", Tools::MoveEntity);
	RenderToolButton("Select", Tools::SelectEntity);
	RenderToolButton("Create", Tools::CreateEntity);
	ImGui::TableNextColumn();
	if (ImGui::Button("Delete")) {
		if (levelTreeEditor.GetSelectedEntity() != NoEntity()) {
			entities.DestroyEntity(levelTreeEditor.GetSelectedEntity());
			levelTreeEditor.CleanTree();
		}
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
			selectedTool = Tools::NoTool;
		}
	}
	ImGui::EndTable();

	ImGui::TableNextColumn();
	{
		static char fileName[41] = "assets/packages/package.pkg";
		if (ImGui::Button("Load Package")) {
			AssetStore& assetStore = GETSYSTEM(AssetStore);
			assetStore.LoadPackage(fileName);
		}
		ImGui::SameLine();
		ImGui::InputText("##LP", fileName, 41);
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

	ImGui::EndTable();
}

void Editor::RenderToolButton(std::string name, Tools tool)
{
	bool isSelected = selectedTool == tool;
	if (isSelected) {
		auto activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
	}
	if (ImGui::Button(name.c_str())) {
		selectedTool = tool;
	}
	if (isSelected) {
		ImGui::PopStyleColor();
	}
}

void Editor::EntityEditor()
{
	auto& entities = GETSYSTEM(Entities);
	auto& registry = entities.GetRegistry();
	auto selectedEntity = levelTreeEditor.GetSelectedEntity();
	if (levelTreeEditor.GetSelectedEntity() != entt::entity(-1)) {
		ROSE_DEFAULT_COMP_EDITOR(GUIDComponent, false);
		ROSE_DEFAULT_COMP_EDITOR(TransformComponent, false);
		RenderComponent<PhysicsBodyComponent, PhysicsEditor>(true, "Physics Body Component", selectedEntity);
		ROSE_DEFAULT_COMP_EDITOR(SpriteComponent, true);
		ROSE_DEFAULT_COMP_EDITOR(CameraComponent, true);
		ROSE_DEFAULT_COMP_EDITOR(AnimationComponent, true);
		RenderComponent<ScriptComponent, ScriptEditor>(true, "Script Component", selectedEntity);
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
