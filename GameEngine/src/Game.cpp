#include "Game.h"

#include <entt/entt.hpp>

#include "Systems.h"
#include "Logger.h"

#include "SdlContainer.h"
#include "AssetStore.h"
#include "Entity.h"
#include "LevelLoader.h"

#include "Transform.h"
#include "Physics.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "TimeSystem.h"
#include "Animation/AnimationSystem.h"
#include "EventSystem/EntityEventSystem.h"
#include "ScriptSystem/ScriptSystem.h"

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
	assetStore.AddTexture("rose", "./assets/Rose.png", 256);
	assetStore.AddTexture("hornet", "./assets/Hornet_Idle.png", 128);
	assetStore.AddTexture("block", "./assets/Block.jpg", 64);
	assetStore.AddTexture("big_ground", "./assets/BigGround.png", 128);
	assetStore.AddTexture("ground", "./assets/Ground.png", 128);
	assetStore.AddTexture("sekiroIdle", "./assets/Sekiro_Idle.png", 32);
	assetStore.LoadAnimation("sekiroIdleAnim", "./assets/AnimationData/Sekiro_Idle.anim");
}

void Game::LoadLevel()
{
	LevelLoader& levelLoader = GETSYSTEM(LevelLoader);
	levelLoader.LoadLevel("SavedLevel.yaml");
	TransformSystem& transformSystem = GETSYSTEM(TransformSystem);
	transformSystem.InitLoaded();
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
	GETSYSTEM(TransformSystem).Update();
	GETSYSTEM(InputSystem).Update();
	GETSYSTEM(AnimationPlayer).Update();
	GETSYSTEM(EntityEventSystem).Update();
	GETSYSTEM(Editor).Update();
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
	transformSystem.DebugRender(renderer.GetWorldToScreenMatrix());
#endif // _DEBUG

#ifdef _EDITOR
	GETSYSTEM(Editor).RenderGizmos();
	GETSYSTEM(Editor).RenderEditor();
#endif
	renderer.Present();
}