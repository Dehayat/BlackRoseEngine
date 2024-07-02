#include "Editor.h"
#include <set>
#include <map>

#include <imgui.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <FileDialog.h>

#include "Core/SdlContainer.h"
#include "Levels/LevelLoader.h"
#include "AssetPipline/AssetStore.h"

#include "Input/InputSystem.h"
#include "Renderer/Renderer.h"
#include "Core/Transform.h"
#include "Animation/AnimationSystem.h"
#include "Events/EntityEventSystem.h"
#include "Scripting/ScriptSystem.h"
#include "Core/TimeSystem.h"
#include "Core/DisableSystem.h"
#include "ImguiSystem.h"

#include "Components/GUIDComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/ScriptComponent.h"
#include "Components/SendEventsToParentComponent.h"
#include "Components/InputComponent.h"

#include "Core/LevelTree.h"
#include "Editor/PhysicsEditor.h"
#include "Editor/ScriptEditor.h"
#include "Editor/InputEditor.h"

#include "DefaultEditor.h"

#define ROSE_DEFAULT_COMP_EDITOR(COMP,DEL)	ROSE_INIT_VARS(COMP);\
											RenderComponent<COMP, DefaultComponentEditor<COMP>>(DEL, #COMP, selectedEntity)



void Editor::Run()
{
	Setup();

#ifdef _DEBUG
	TransformSystem& transformSystem = ROSE_GETSYSTEM(TransformSystem);
	transformSystem.InitDebugDrawer();
	PhysicsSystem& physics = ROSE_GETSYSTEM(PhysicsSystem);
	physics.InitDebugDrawer();
	physics.EnableDebug(true);
	transformSystem.EnableDebug(true);
#endif // !_DEBUG

	isRunning = true;
	while(isRunning)
	{
		Update();
		Render();
	}
}


Editor::Editor():BaseGame()
{
	SetupImgui();
	Reset();
	isGameRunning = false;
	selectedTool = Tools::SelectEntity;
	lastTool = selectedTool;
	gizmosSetting = Gizmos::ALL;
	moveTool = MoveTool(ROSE_GETSYSTEM(SdlContainer).GetRenderer());
	rotateTool = RotateTool(ROSE_GETSYSTEM(SdlContainer).GetRenderer());
}

void Editor::SetupImgui()
{
	ROSE_CREATESYSTEM(ImguiSystem);
}

Editor::~Editor()
{
	CloseImgui();
}

void Editor::Reset()
{
	createdEntity = NoEntity();
}

void Editor::CloseImgui()
{
	ROSE_DESTROYSYSTEM(ImguiSystem);
}

void Editor::Update()
{
	bool exitGame = ProcessEvents();
	if(exitGame)
	{
		isRunning = false;
	}
	bool isGameRunning = IsGameRunning();
	ROSE_GETSYSTEM(TimeSystem).Update();
	ROSE_GETSYSTEM(TransformSystem).Update();
	ROSE_GETSYSTEM(InputSystem).Update();
	if(isGameRunning)
	{
		ROSE_GETSYSTEM(PhysicsSystem).Update();
	}
	ROSE_GETSYSTEM(AnimationSystem).Update();
	if(isGameRunning)
	{
		ROSE_GETSYSTEM(ScriptSystem).Update();
		ROSE_GETSYSTEM(EntityEventSystem).Update();
	}

	if(!ROSE_GETSYSTEM(ImguiSystem).IsMouseCaptured())
	{
		UpdateViewportControls();
		if(selectedTool == Tools::MoveEntity)
		{
			moveTool.Update();
		} else if(selectedTool == Tools::RotateEntity)
		{
			rotateTool.Update();
		}
	}
	if(!ImGui::IsAnyItemActive())
	{
		UpdateGlobalControls();
	}
}
void Editor::Render()
{

	RendererSystem& renderer = ROSE_GETSYSTEM(RendererSystem);
	renderer.Render();
	TransformSystem& transformSystem = ROSE_GETSYSTEM(TransformSystem);
	transformSystem.GetDebugRenderer().SetMatrix(renderer.GetWorldToScreenMatrix());
	if(GetGizmos() == Gizmos::TRANSFORM || GetGizmos() == Gizmos::ALL)
	{
		transformSystem.DebugRender(renderer.GetWorldToScreenMatrix(), GetSelectedEntity());
	}
	if(GetGizmos() == Gizmos::PHYSICS || GetGizmos() == Gizmos::ALL)
	{
		ROSE_GETSYSTEM(PhysicsSystem).DebugRender(renderer.GetWorldToScreenMatrix());
	}
	RenderGizmos();
	RenderEditor();
	renderer.Present();
}

void Editor::UpdateViewportControls()
{
	auto& entities = ROSE_GETSYSTEM(EntitySystem);
	auto& registry = entities.GetRegistry();
	auto& input = ROSE_GETSYSTEM(InputSystem);
	auto& gameRenderer = ROSE_GETSYSTEM(RendererSystem);
	if(selectedTool == Tools::CreateEntity)
	{
		auto mousePos = glm::vec3(input.GetMousePosition(), 1) * gameRenderer.GetScreenToWorldMatrix();
		if(input.GetMouseButton(LEFT_BUTTON).justPressed)
		{
			ROSE_LOG("entity created");
			createdEntity = entities.CreateEntity();
			levelTreeEditor.SelectEntity(createdEntity);
			registry.emplace<TransformComponent>(createdEntity, glm::vec2(mousePos.x, mousePos.y), glm::vec2(1, 1), 0);
		}
		if(createdEntity != entt::entity(-1))
		{
			if(input.GetMouseButton(LEFT_BUTTON).justReleased)
			{
				createdEntity = entt::entity(-1);
			}
			if(input.GetMouseButton(LEFT_BUTTON).isPressed)
			{
				registry.get<TransformComponent>(createdEntity).globalPosition = glm::vec2(mousePos.x, mousePos.y);
				registry.get<TransformComponent>(createdEntity).UpdateLocals();
			}
		}
	}
	if(selectedTool == Tools::SelectEntity)
	{
		UpdateSelectTool();
	}
	if(selectedTool == Tools::ParentEntity)
	{
		UpdateSelectTool();
		UpdateParentTool();
	}
}

void Editor::UpdateGlobalControls()
{
	auto& entities = ROSE_GETSYSTEM(EntitySystem);
	auto& input = ROSE_GETSYSTEM(InputSystem);
	auto& levelLoader = ROSE_GETSYSTEM(LevelLoader);
	if(input.GetKey(InputKey::LCTRL).isPressed || input.GetKey(InputKey::RCTRL).isPressed)
	{
		if(input.GetKey(InputKey::N).justPressed)
		{
			levelLoader.UnloadLevel();
		}
		if(input.GetKey(InputKey::O).justPressed)
		{
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("yaml");
			if(fileName != "")
			{
				levelLoader.UnloadLevel();
				levelLoader.LoadLevel(fileName);
				ROSE_GETSYSTEM(RendererSystem).InitLoaded();
			}
		}
		if(input.GetKey(InputKey::S).justPressed)
		{
			if(input.GetKey(InputKey::LSHIFT).isPressed || input.GetKey(InputKey::RSHIFT).isPressed)
			{
				auto fileName = ROSE_GETSYSTEM(FileDialog).SaveFile("yaml");
				if(fileName != "")
				{
					levelLoader.SaveLevel(fileName);
				}
			} else
			{
				levelLoader.SaveLevel(levelLoader.GetCurrentLevelFile());
			}
		}
		if(input.GetKey(InputKey::D).justPressed)
		{
			if(GetSelectedEntity() != NoEntity())
			{
				levelTreeEditor.SelectEntity(ROSE_GETSYSTEM(EntitySystem).CopyEntity(GetSelectedEntity()));
			}
		}
		return;
	}
	static vec2 lastPos;
	static auto panCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	static bool isPanning = false;
	if(input.GetMouseButton(InputMouse::MIDDLE_BUTTON).justPressed)
	{
		isPanning = true;
		lastPos = input.GetMousePosition();
	}
	if(input.GetMouseButton(InputMouse::MIDDLE_BUTTON).isPressed)
	{
		SDL_SetCursor(panCursor);
		auto offset = lastPos - input.GetMousePosition();
		auto& renderSystem = ROSE_GETSYSTEM(RendererSystem);
		renderSystem.editorViewPos += TransformComponent::GetDir(renderSystem.GetScreenToWorldMatrix(), offset);
		lastPos = input.GetMousePosition();
	} else
	{
		if(isPanning)
		{
			SDL_SetCursor(SDL_GetDefaultCursor());
			isPanning = false;
		}
	}
	if(GetSelectedEntity() != NoEntity())
	{
		if(input.GetKey(InputKey::DELETE).justReleased)
		{
			if(levelTreeEditor.GetSelectedEntity() != NoEntity())
			{
				entities.DestroyEntity(levelTreeEditor.GetSelectedEntity());
			}
		}
	}

	if(input.GetKey(InputKey::C).justPressed)
	{
		selectedTool = Tools::CreateEntity;
	}
	if(input.GetKey(InputKey::W).justPressed)
	{
		selectedTool = Tools::MoveEntity;
	}
	if(input.GetKey(InputKey::E).justPressed)
	{
		selectedTool = Tools::RotateEntity;
	}
	if(input.GetKey(InputKey::Q).justPressed)
	{
		selectedTool = Tools::SelectEntity;
	}
	if(input.GetKey(InputKey::X).justPressed)
	{
		selectedTool = Tools::NoTool;
	}
	if(input.GetKey(InputKey::P).justPressed)
	{
		if(input.GetKey(RSHIFT).isPressed || input.GetKey(LSHIFT).isPressed)
		{
			if(input.GetKey(RSHIFT).isPressed || input.GetKey(LSHIFT).isPressed)
			{
				if(GetSelectedEntity() != NoEntity())
				{
					ROSE_GETSYSTEM(LevelTree).RemoveParent(GetSelectedEntity());
				}
			}
		} else
		{
			if(GetSelectedEntity() != NoEntity())
			{
				lastTool = selectedTool;
				selectedTool = Tools::ParentEntity;
				childEntity = GetSelectedEntity();
			}
		}
	}
	if(input.GetKey(InputKey::G).justPressed)
	{
		gizmosSetting = gizmosSetting + 1;
		if(gizmosSetting > Gizmos::ALL)
		{
			gizmosSetting = Gizmos::NONE;
		}
	}

	if(selectedTool == Tools::ParentEntity)
	{
		UpdateParentTool();
	}
}
int Editor::GetGizmos()
{
	return gizmosSetting;
}


static bool IsPointInsideRect(vec2 point, SDL_FRect rect)
{
	if(point.x<rect.x || point.x > rect.x + rect.w)
	{
		return false;
	}
	if(point.y<rect.y || point.y > rect.y + rect.h)
	{
		return false;
	}
	return true;
}
static bool IsPointInsideCircle(vec2 point, vec2 center, float radius)
{
	return distance(point, center) < radius;
}

std::set<entt::entity> skipSelect;
std::map<entt::entity, float> skipTimer;
void Editor::UpdateSelectTool()
{
	auto& entities = ROSE_GETSYSTEM(EntitySystem);
	auto& renderer = ROSE_GETSYSTEM(RendererSystem);
	auto& registry = entities.GetRegistry();
	auto& input = ROSE_GETSYSTEM(InputSystem);
	auto& timeSys = ROSE_GETSYSTEM(TimeSystem);
	if(input.GetMouseButton(LEFT_BUTTON).justPressed)
	{
		auto mousePos = input.GetMousePosition();
		auto view = registry.view<const SpriteComponent>();
		bool selected = false;
		for(auto entity : view)
		{
			if(skipSelect.find(entity) != skipSelect.end())
			{
				continue;
			}
			auto& sprite = registry.get<SpriteComponent>(entity);
			if(IsPointInsideRect(mousePos, sprite.destRect))
			{
				levelTreeEditor.SelectEntity(entity);
				selected = true;
			}
		}
		auto view2 = registry.view<const TransformComponent>();
		for(auto entity : view2)
		{
			if(skipSelect.find(entity) != skipSelect.end())
			{
				continue;
			}
			auto& trx = registry.get<TransformComponent>(entity);
			auto matrix = renderer.GetWorldToScreenMatrix();
			auto center = TransformComponent::GetPosition(matrix, trx.globalPosition);
			if(IsPointInsideCircle(mousePos, center, 30))
			{
				levelTreeEditor.SelectEntity(entity);
				selected = true;
			}
		}
		float t = timeSys.GetTime();
		if(!selected)
		{
			levelTreeEditor.SelectEntity(NoEntity());
		} else
		{
			auto selectedEntity = levelTreeEditor.GetSelectedEntity();
			skipSelect.insert(selectedEntity);
			skipTimer[selectedEntity] = t + 0.3f;
		}
		std::vector<entt::entity> remove;
		for(auto entity : skipTimer)
		{
			if(entity.second < t)
			{
				skipSelect.erase(entity.first);
				remove.push_back(entity.first);
			}
		}
		for(auto entity : remove)
		{
			skipTimer.erase(entity);
		}
		remove.clear();
	}
}

void Editor::UpdateParentTool()
{
	auto& input = ROSE_GETSYSTEM(InputSystem);
	if(input.GetKey(InputKey::ESC).justPressed)
	{
		childEntity = NoEntity();
	}
	static auto panCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	if(SDL_GetCursor() != panCursor)
	{
		SDL_SetCursor(panCursor);
	}

	if(childEntity != NoEntity() && childEntity != GetSelectedEntity())
	{
		if(GetSelectedEntity() != NoEntity())
		{
			ROSE_GETSYSTEM(LevelTree).TrySetParent(childEntity, GetSelectedEntity());
			childEntity = NoEntity();
		}
	}
	if(childEntity == NoEntity() || GetSelectedEntity() == NoEntity())
	{
		SDL_SetCursor(SDL_GetDefaultCursor());
		selectedTool = lastTool;
		if(selectedTool == Tools::ParentEntity)
		{
			selectedTool = Tools::SelectEntity;
		}
	}
}

bool Editor::ProcessEvents()
{
	bool exit = false;
	SDL_Event sdlEvent;
	while(SDL_PollEvent(&sdlEvent))
	{
		switch(sdlEvent.type)
		{
		case SDL_QUIT:
			exit = true;
			break;
		case SDL_DROPFILE:
			HandleDropFile(sdlEvent);
			break;
		case SDL_MOUSEWHEEL:
			UpdateScrollWheel(sdlEvent);
			break;
		default:
			break;
		}
		ROSE_GETSYSTEM(ImguiSystem).HandleEvent(sdlEvent);
	}
	return exit;
}

void Editor::UpdateScrollWheel(SDL_Event& sdlEvent)
{
	if(ROSE_GETSYSTEM(ImguiSystem).IsMouseCaptured())
	{
		return;
	}
	auto& input = ROSE_GETSYSTEM(InputSystem);
	auto& renderSystem = ROSE_GETSYSTEM(RendererSystem);
	if(input.GetKey(InputKey::LSHIFT).isPressed)
	{
		renderSystem.editorViewHeight -= sdlEvent.wheel.preciseY * 2;
	} else
	{
		renderSystem.editorViewHeight -= sdlEvent.wheel.preciseY * 0.5;
	}
}

void Editor::HandleDropFile(SDL_Event& sdlEvent)
{
	auto fileType = ROSE_GETSYSTEM(FileDialog).GetFileExtension(sdlEvent.drop.file);
	if(fileType == ".pro")
	{
		LoadProject(sdlEvent.drop.file);
	} else if(fileType == ".pkg")
	{
		ROSE_GETSYSTEM(AssetStore).LoadPackage(sdlEvent.drop.file);
	} else if(fileType == ".yaml")
	{
		ROSE_GETSYSTEM(LevelLoader).UnloadLevel();
		ROSE_GETSYSTEM(LevelLoader).LoadLevel(sdlEvent.drop.file);
	}
}
void Editor::RenderGizmos()
{
	auto& regsitry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& rendererSystem = ROSE_GETSYSTEM(RendererSystem);
	auto renderer = ROSE_GETSYSTEM(SdlContainer).GetRenderer();
	auto camView = regsitry.view<CameraComponent, TransformComponent>();
	for(auto entity : camView)
	{
		auto height = camView.get<CameraComponent>(entity).height;
		auto width = rendererSystem.GetAspectRatio() * height;
		auto& trx = camView.get<TransformComponent>(entity);
		auto matrix = rendererSystem.GetWorldToScreenMatrix();
		auto rotateMatrix = TransformComponent::MakeRotMatrix(trx.globalRotation);
		auto center = TransformComponent::GetPosition(matrix, trx.globalPosition);
		auto scalledSize = TransformComponent::GetDir(matrix, {width,height});
		auto size = abs(scalledSize);


		static int16_t polygon[2][4];
		auto localCamToScreenMatrix = rotateMatrix * matrix;
		auto edge = trx.globalPosition + vec2(-width / 2, height / 2);
		edge = TransformComponent::GetPosition(localCamToScreenMatrix, edge);
		polygon[0][0] = edge.x;
		polygon[1][0] = edge.y;

		edge = trx.globalPosition + vec2(width / 2, height / 2);
		edge = TransformComponent::GetPosition(localCamToScreenMatrix, edge);
		polygon[0][1] = edge.x;
		polygon[1][1] = edge.y;

		edge = trx.globalPosition + vec2(width / 2, -height / 2);
		edge = TransformComponent::GetPosition(localCamToScreenMatrix, edge);
		polygon[0][2] = edge.x;
		polygon[1][2] = edge.y;

		edge = trx.globalPosition + vec2(-width / 2, -height / 2);
		edge = TransformComponent::GetPosition(localCamToScreenMatrix, edge);
		polygon[0][3] = edge.x;
		polygon[1][3] = edge.y;


		if(levelTreeEditor.GetSelectedEntity() == entity)
		{
			filledCircleRGBA(renderer, center.x, center.y, size.y / 10, 200, 200, 0, 30);
			polygonRGBA(renderer, polygon[0], polygon[1], 4, 200, 200, 0, 200);
			filledPolygonRGBA(renderer, polygon[0], polygon[1], 4, 200, 200, 200, 100);
			circleRGBA(renderer, center.x, center.y, size.y / 10, 200, 200, 0, 150);
			circleRGBA(renderer, center.x, center.y, size.y / 20, 200, 200, 0, 150);
		} else
		{
			filledCircleRGBA(renderer, center.x, center.y, size.y / 10, 200, 200, 200, 30);
			circleRGBA(renderer, center.x, center.y, size.y / 10, 200, 200, 200, 150);
			circleRGBA(renderer, center.x, center.y, size.y / 20, 200, 200, 200, 150);
			polygonRGBA(renderer, polygon[0], polygon[1], 4, 200, 200, 200, 150);
		}

	}

	if(selectedTool == Tools::MoveEntity)
	{
		moveTool.SetSelectedEntity(levelTreeEditor.GetSelectedEntity());
		moveTool.RenderGizmos();
	}
	if(selectedTool == Tools::RotateEntity)
	{
		rotateTool.SetSelectedEntity(levelTreeEditor.GetSelectedEntity());
		rotateTool.RenderGizmos();
	}
}

void Editor::RenderEditor()
{
	RenderImgui();

	int w, h;
	SDL_GetWindowSize(ROSE_GETSYSTEM(SdlContainer).GetWindow(), &w, &h);

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
	auto& entities = ROSE_GETSYSTEM(EntitySystem);
	auto& registry = entities.GetRegistry();
	auto& input = ROSE_GETSYSTEM(InputSystem);
	auto& gameRenderer = ROSE_GETSYSTEM(RendererSystem);
	auto& levelLoader = ROSE_GETSYSTEM(LevelLoader);
	ImGui::BeginTable("ToolBar", 3, ImGuiTableFlags_BordersInnerV);
	ImGui::TableNextColumn();
	ImGui::BeginTable("Tools", 3, ImGuiTableFlags_BordersInnerV);
	ImGui::TableNextColumn();
	RenderToolButton("Move", Tools::MoveEntity);
	ImGui::SameLine();
	RenderToolButton("Rotate", Tools::RotateEntity);
	RenderToolButton("Select", Tools::SelectEntity);
	RenderToolButton("Create", Tools::CreateEntity);
	ImGui::TableNextColumn();
	if(ImGui::Button("Delete"))
	{
		if(levelTreeEditor.GetSelectedEntity() != NoEntity())
		{
			entities.DestroyEntity(levelTreeEditor.GetSelectedEntity());
		}
	}

	ImGui::TableNextColumn();
	if(IsGameRunning())
	{
		if(ImGui::Button("Stop"))
		{
			isGameRunning = false;
		}
	} else
	{
		if(ImGui::Button("Play"))
		{
			isGameRunning = true;
			selectedTool = Tools::NoTool;
		}
	}
	ImGui::EndTable();

	ImGui::TableNextColumn();
	{
		if(ImGui::Button("Load Package"))
		{
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("pkg");
			if(fileName != "")
			{
				AssetStore& assetStore = ROSE_GETSYSTEM(AssetStore);
				assetStore.LoadPackage(fileName);
			}
		}
		ImGui::SameLine();
	}

	ImGui::TableNextColumn();
	auto view = registry.view<const GUIDComponent, TransformComponent>();
	{
		if(ImGui::Button("Load Level"))
		{
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("yaml");
			if(fileName != "")
			{
				levelLoader.UnloadLevel();
				levelLoader.LoadLevel(fileName);
				ROSE_GETSYSTEM(RendererSystem).InitLoaded();
			}
		}
	}
	{
		if(ImGui::Button("Save Level"))
		{
			if(levelLoader.GetCurrentLevelFile() != "")
			{
				levelLoader.SaveLevel(levelLoader.GetCurrentLevelFile());
			}
		}
	}
	{
		if(ImGui::Button("Save Level As"))
		{
			auto fileName = ROSE_GETSYSTEM(FileDialog).SaveFile("yaml");
			if(fileName != "")
			{
				levelLoader.SaveLevel(fileName);
			}
		}
		ImGui::SameLine();
	}

	ImGui::EndTable();
}

void Editor::RenderToolButton(std::string name, Tools tool)
{
	bool isSelected = selectedTool == tool;
	if(isSelected)
	{
		auto activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
	}
	if(ImGui::Button(name.c_str()))
	{
		selectedTool = tool;
	}
	if(isSelected)
	{
		ImGui::PopStyleColor();
	}
}

void Editor::EntityEditor()
{
	auto& entities = ROSE_GETSYSTEM(EntitySystem);
	auto& registry = entities.GetRegistry();
	auto selectedEntity = levelTreeEditor.GetSelectedEntity();
	if(levelTreeEditor.GetSelectedEntity() != entt::entity(-1))
	{
		RenderEntityEditor(selectedEntity);
		ROSE_DEFAULT_COMP_EDITOR(GUIDComponent, false);
		ROSE_DEFAULT_COMP_EDITOR(TransformComponent, false);
		RenderComponent<PhysicsBodyComponent, PhysicsEditor>(true, "Physics Body Component", selectedEntity);
		ROSE_DEFAULT_COMP_EDITOR(SpriteComponent, true);
		ROSE_DEFAULT_COMP_EDITOR(CameraComponent, true);
		ROSE_DEFAULT_COMP_EDITOR(AnimationComponent, true);
		RenderComponent<ScriptComponent, ScriptEditor>(true, "Script Component", selectedEntity);
		ROSE_DEFAULT_COMP_EDITOR(SendEventsToParentComponent, true);
		RenderComponent<InputComponent, InputEditor>(true, "Input Component", selectedEntity);
	}
}
void Editor::RenderEntityEditor(entt::entity entity)
{
	static bool enabled = true;
	auto& entities = ROSE_GETSYSTEM(EntitySystem);
	auto& registry = entities.GetRegistry();
	if(registry.any_of<DisableComponent>(entity))
	{
		enabled = !registry.get<DisableComponent>(entity).selfDisabled;
	} else
	{
		enabled = true;
	}
	ImGui::SeparatorText("Entity");
	if(ImGui::Checkbox("Enabled", &enabled))
	{
		if(enabled)
		{
			ROSE_GETSYSTEM(DisableSystem).Enable(entity);
		} else
		{
			ROSE_GETSYSTEM(DisableSystem).Disable(entity);
		}
	}
}


void Editor::PresentImGui()
{
	ROSE_GETSYSTEM(ImguiSystem).Present();
}

void Editor::RenderImgui()
{
	ROSE_GETSYSTEM(ImguiSystem).Render();
}
