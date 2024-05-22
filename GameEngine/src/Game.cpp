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

Game::Game() {
	Logger::Log("Game Constructed");
	SetupBaseSystems();
	SetupLowLevelSystems();
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
	TransformSystem& transformSystem = CREATESYSTEM(TransformSystem);
	transformSystem.InitDebugDrawer();
	PhysicsSystem& physics = CREATESYSTEM(PhysicsSystem, 0, -10);
	physics.InitDebugDrawer();

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
	RendererSystem& renderer = GETSYSTEM(RendererSystem);
	renderer.InitLoaded();
}

void Game::LoadAssets()
{
	AssetStore& assetStore = GETSYSTEM(AssetStore);
	assetStore.AddTexture("rose", "./assets/Rose.png", 256);
	assetStore.AddTexture("hornet", "./assets/Hornet_Idle.png", 128);
	assetStore.AddTexture("block", "./assets/Block.jpg", 64);
	assetStore.AddTexture("big_ground", "./assets/BigGround.png", 128);
	assetStore.AddTexture("ground", "./assets/Ground.png", 128);
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
	bool exitGame = GETSYSTEM(SdlContainer).ProcessEvents();
	if (exitGame) {
		isRunning = false;
	}

	GETSYSTEM(TimeSystem).Update();
	GETSYSTEM(TransformSystem).Update();
	GETSYSTEM(InputSystem).Update();
	GETSYSTEM(PhysicsSystem).Update();
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
	renderer.Present();
}