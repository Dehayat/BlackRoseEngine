#include "Game.h"

#include <entt/entt.hpp>

#include "Systems.h"
#include "Debugging/Logger.h"

#include "SdlContainer.h"
#include "AssetStore/AssetStore.h"
#include "Entity.h"
#include "LevelLoader.h"

#include "Transform.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "TimeSystem.h"
#include "Animation/AnimationSystem.h"
#include "Events/EntityEventSystem.h"
#include "Scripting/ScriptSystem.h"
#include "Editor/LevelTree.h"

#ifdef _EDITOR
#include "Editor/Editor.h"
#endif

Game::Game() {
	Logger::Log("Game Constructed");
	SetupBaseSystems();
	SetupLowLevelSystems();
#ifdef _EDITOR
	CREATESYSTEM(Editor);
#endif
}

Game::~Game() {
	Logger::Log("Game Destructed");
}

void Game::SetupBaseSystems() {
	CREATESYSTEM(SdlContainer, 1200, (float)1200 * 9 / 16);
	CREATESYSTEM(LevelLoader);
	CREATESYSTEM(Entities);
	CREATESYSTEM(AssetStore);
}

void Game::SetupLowLevelSystems()
{
	CREATESYSTEM(LevelTree);
	CREATESYSTEM(TimeSystem);
	CREATESYSTEM(InputSystem);
	CREATESYSTEM(RendererSystem);
	CREATESYSTEM(AnimationPlayer);
	TransformSystem& transformSystem = CREATESYSTEM(TransformSystem);
	transformSystem.InitDebugDrawer();
	PhysicsSystem& physics = CREATESYSTEM(PhysicsSystem, 0, -10);
	physics.InitDebugDrawer();
	CREATESYSTEM(EntityEventSystem);
	CREATESYSTEM(ScriptSystem);

#ifdef _DEBUG
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
	GETSYSTEM(RendererSystem).InitLoaded();
#ifdef _EDITOR
	GETSYSTEM(Editor).Reset();
#endif
}

void Game::LoadAssets()
{
	AssetStore& assetStore = GETSYSTEM(AssetStore);
	assetStore.LoadPackage("assets/Packages/a.pkg");
}

void Game::LoadLevel()
{
	LevelLoader& levelLoader = GETSYSTEM(LevelLoader);
	levelLoader.LoadLevel("assets/Levels/Level1.yaml");
}

void Game::Update()
{
#ifdef _EDITOR
	bool exitGame = GETSYSTEM(Editor).ProcessEvents();
#else
	bool exitGame = GETSYSTEM(SdlContainer).ProcessEvents();
#endif
	if (exitGame) {
		isRunning = false;
	}
#ifdef _EDITOR
	GETSYSTEM(Editor).Update();
	bool isGameRunning = GETSYSTEM(Editor).IsGameRunning();
	GETSYSTEM(TimeSystem).Update();
	GETSYSTEM(TransformSystem).Update();
	GETSYSTEM(InputSystem).Update();
	if (isGameRunning) {
		GETSYSTEM(PhysicsSystem).Update();
	}
	GETSYSTEM(AnimationPlayer).Update();
	if (isGameRunning) {
		GETSYSTEM(EntityEventSystem).Update();
		GETSYSTEM(ScriptSystem).Update();
	}
#else
	GETSYSTEM(TimeSystem).Update();
	GETSYSTEM(TransformSystem).Update();
	GETSYSTEM(InputSystem).Update();
	GETSYSTEM(PhysicsSystem).Update();
	GETSYSTEM(AnimationPlayer).Update();
	GETSYSTEM(EntityEventSystem).Update();
	GETSYSTEM(ScriptSystem).Update();
#endif
}

void Game::Render()
{

	RendererSystem& renderer = GETSYSTEM(RendererSystem);
	renderer.Render();
#ifdef _DEBUG
	GETSYSTEM(PhysicsSystem).DebugRender(renderer.GetWorldToScreenMatrix());

	TransformSystem& transformSystem = GETSYSTEM(TransformSystem);
	transformSystem.GetDebugRenderer().SetMatrix(renderer.GetWorldToScreenMatrix());
#ifndef _EDITOR
	transformSystem.DebugRender(renderer.GetWorldToScreenMatrix(), NoEntity());
#endif
#endif // _DEBUG

#ifdef _EDITOR
	auto& editor = GETSYSTEM(Editor);
	transformSystem.DebugRender(renderer.GetWorldToScreenMatrix(), editor.GetSelectedEntity());
	editor.RenderGizmos();
	editor.RenderEditor();
#endif
	renderer.Present();
}