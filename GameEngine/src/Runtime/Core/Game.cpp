#include "Game.h"

#include "Core/Systems.h"
#include "Core/Reflection.h"
#include "Core/Log.h"

#include <FileDialog.h>
#include "Core/SdlContainer.h"
#include "AssetPipline/AssetStore.h"
#include "Core/Entity.h"
#include "Project/ProjectLoader.h"
#include "Levels/LevelLoader.h"

#include "Core/Transform.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "Core/TimeSystem.h"
#include "Animation/AnimationSystem.h"
#include "Events/EntityEventSystem.h"
#include "Scripting/ScriptSystem.h"
#include "Editor/LevelTree.h"

#ifdef _EDITOR
#include "Editor/Editor.h"
#endif

Game::Game() {
	ROSE_LOG("Game constructed");
	SetupBaseSystems();
	SetupLowLevelSystems();
#ifdef _EDITOR
	ROSE_CREATESYSTEM(Editor);
#endif
}

Game::~Game() {
	ROSE_LOG("Game destrcuted");
}

void Game::SetupBaseSystems() {
	ROSE_CREATESYSTEM(FileDialog);
	ROSE_CREATESYSTEM(SdlContainer, 1200, (float)1200 * 9 / 16);
	ROSE_CREATESYSTEM(ProjectLoader);
	ROSE_CREATESYSTEM(LevelLoader);
	ROSE_CREATESYSTEM(Entities);
	ROSE_CREATESYSTEM(AssetStore);
	ROSE_CREATESYSTEM(ReflectionSystem);
}

void Game::SetupLowLevelSystems()
{
	ROSE_CREATESYSTEM(LevelTree);
	ROSE_CREATESYSTEM(TimeSystem);
	ROSE_CREATESYSTEM(InputSystem);
	ROSE_CREATESYSTEM(RendererSystem);
	ROSE_CREATESYSTEM(AnimationPlayer);
	ROSE_CREATESYSTEM(EntityEventSystem);
	ROSE_CREATESYSTEM(ScriptSystem);
	ROSE_CREATESYSTEM(TransformSystem);
	ROSE_CREATESYSTEM(PhysicsSystem, 0, -10);

#ifdef _DEBUG
	TransformSystem& transformSystem = ROSE_GETSYSTEM(TransformSystem);
	transformSystem.InitDebugDrawer();
	PhysicsSystem& physics = ROSE_GETSYSTEM(PhysicsSystem);
	physics.InitDebugDrawer();
	physics.EnableDebug(true);
	transformSystem.EnableDebug(true);
#endif // !_DEBUG

	isRunning = false;
}

void Game::Run()
{
	Setup();
	isRunning = true;
	while (isRunning) {
		Update();
		Render();
	}
}

void Game::Setup()
{
	LoadAssets();
	LoadLevel();
	ROSE_GETSYSTEM(RendererSystem).InitLoaded();
#ifdef _EDITOR
	ROSE_GETSYSTEM(Editor).Reset();
#endif
}

void Game::LoadAssets()
{
	ROSE_GETSYSTEM(ProjectLoader).LoadProject("p.pro");
}

void Game::LoadLevel()
{
	auto project = ROSE_GETSYSTEM(ProjectLoader).GetCurrentProject();
	auto startLevelIndex = project->GetStartLevel();
	if (startLevelIndex != -1) {
		LevelLoader& levelLoader = ROSE_GETSYSTEM(LevelLoader);
		levelLoader.LoadLevel(project->GetLevelFile(startLevelIndex));
	}
}

void Game::Update()
{
#ifdef _EDITOR
	bool exitGame = ROSE_GETSYSTEM(Editor).ProcessEvents();
#else
	bool exitGame = ROSE_GETSYSTEM(SdlContainer).ProcessEvents();
#endif
	if (exitGame) {
		isRunning = false;
}
#ifdef _EDITOR
	ROSE_GETSYSTEM(Editor).Update();
	bool isGameRunning = ROSE_GETSYSTEM(Editor).IsGameRunning();
	ROSE_GETSYSTEM(TimeSystem).Update();
	ROSE_GETSYSTEM(TransformSystem).Update();
	ROSE_GETSYSTEM(InputSystem).Update();
	if (isGameRunning) {
		ROSE_GETSYSTEM(PhysicsSystem).Update();
	}
	ROSE_GETSYSTEM(AnimationPlayer).Update();
	if (isGameRunning) {
		ROSE_GETSYSTEM(EntityEventSystem).Update();
		ROSE_GETSYSTEM(ScriptSystem).Update();
	}
#else
	ROSE_GETSYSTEM(TimeSystem).Update();
	ROSE_GETSYSTEM(TransformSystem).Update();
	ROSE_GETSYSTEM(InputSystem).Update();
	ROSE_GETSYSTEM(PhysicsSystem).Update();
	ROSE_GETSYSTEM(AnimationPlayer).Update();
	ROSE_GETSYSTEM(EntityEventSystem).Update();
	ROSE_GETSYSTEM(ScriptSystem).Update();
#endif
}

void Game::Render()
{

	RendererSystem& renderer = ROSE_GETSYSTEM(RendererSystem);
	renderer.Render();
#ifdef _DEBUG
	ROSE_GETSYSTEM(PhysicsSystem).DebugRender(renderer.GetWorldToScreenMatrix());
#endif // _DEBUG

#ifdef _EDITOR
	TransformSystem& transformSystem = ROSE_GETSYSTEM(TransformSystem);
	transformSystem.GetDebugRenderer().SetMatrix(renderer.GetWorldToScreenMatrix());
	transformSystem.DebugRender(renderer.GetWorldToScreenMatrix(), NoEntity());
	auto& editor = ROSE_GETSYSTEM(Editor);
	transformSystem.DebugRender(renderer.GetWorldToScreenMatrix(), editor.GetSelectedEntity());
	editor.RenderGizmos();
	editor.RenderEditor();
#endif
	renderer.Present();
}