#include "Game.h"
#include <sstream>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ryml/ryml.hpp>
#include "SdlContainer.h"
#include "AssetStore.h"
#include "Transform.h"
#include "Physics.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "LevelLoader.h"
#include "TimeSystem.h"
#include "Components/GUIDComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Logger.h"


Game::Game() {
	Logger::Log("Game Constructed");
	SetupBaseSystems();
	SetupLowLevelSystems();
}

Game::~Game() {
	Logger::Log("Game Destructed");
}

void Game::SetupBaseSystems() {
	entt::locator<SdlContainer>::emplace<SdlContainer>(1200, (float)1200 * 9 / 16);
	entt::locator<AssetStore>::emplace<AssetStore>();
	entt::locator<Entities>::emplace<Entities>();
	entt::locator<LevelLoader>::emplace<LevelLoader>();
}

void Game::SetupLowLevelSystems()
{
	TransformSystem& transformSystem = entt::locator<TransformSystem>::emplace<TransformSystem>();
	transformSystem.InitDebugDrawer();
	PhysicsSystem& physics = entt::locator<PhysicsSystem>::emplace<PhysicsSystem>(0, -10);
	physics.InitDebugDrawer();
	entt::locator<RendererSystem>::emplace<RendererSystem>();
	entt::locator<InputSystem>::emplace<InputSystem>();
	entt::locator<TimeSystem>::emplace<TimeSystem>();

#ifdef _DEBUG
	physics.EnableDebug(true);
	transformSystem.EnableDebug(true);
#endif // !_DEBUG

	isRunning = false;
}

void Game::LoadAssets()
{
	AssetStore& assetStore = entt::locator<AssetStore>::value();
	assetStore.AddTexture("rose", "./assets/Rose.png", 512);
	assetStore.AddTexture("hornet", "./assets/Hornet_Idle.png", 128);
	assetStore.AddTexture("block", "./assets/Block.jpg", 64);
	assetStore.AddTexture("big_ground", "./assets/BigGround.png", 128);
}

void Game::LoadLevel()
{
	LevelLoader& levelLoader = entt::locator<LevelLoader>::value();
	levelLoader.LoadLevel("SavedLevel.yaml");
	RegisterAllEntities();
	TransformSystem& transformSystem = entt::locator<TransformSystem>::value();
	transformSystem.InitLoaded();
}

void Game::Setup()
{
	LoadAssets();
	LoadLevel();
	RendererSystem& renderer = entt::locator<RendererSystem>::value();
	renderer.InitLoaded();


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
void Game::Update()
{
	TimeSystem& timeSystem = entt::locator<TimeSystem>::value();
	timeSystem.Update();
	TransformSystem& transformSystem = entt::locator<TransformSystem>::value();
	transformSystem.Update();

	if (entt::locator<SdlContainer>::value().ProcessEvents()) {
		isRunning = false;
	}
	InputSystem& input = entt::locator<InputSystem>::value();
	input.Update(entt::locator<SdlContainer>::value().GetWindow());

	PhysicsSystem& physics = entt::locator<PhysicsSystem>::value();
	physics.Update();

	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	auto view2 = registry.view<PlayerComponent, const TransformComponent, PhysicsBodyComponent>();
	for (auto entity : view2) {
		const auto& pos = view2.get<TransformComponent>(entity);
		auto& phys = view2.get<PhysicsBodyComponent>(entity);
		auto& player = view2.get<PlayerComponent>(entity);
		if (input.GetKey(InputKey::LEFT).isPressed || input.GetKey(InputKey::A).isPressed) {
			player.input = -1;
		}
		else if (input.GetKey(InputKey::RIGHT).isPressed || input.GetKey(InputKey::D).isPressed) {
			player.input = 1;
		}
		else {
			player.input = 0;
		}
		auto vel = b2Vec2(player.speed * player.input, phys.body->GetLinearVelocity().y);
		phys.body->SetLinearVelocity(vel);
	}
}

void Game::Render()
{

	RendererSystem& renderer = entt::locator<RendererSystem>::value();
	renderer.Render();
#ifdef _DEBUG
	PhysicsSystem& physics = entt::locator<PhysicsSystem>::value();
	physics.DebugRender(renderer.GetWorldToScreenMatrix());

	TransformSystem& transformSystem = entt::locator<TransformSystem>::value();
	transformSystem.GetDebugRenderer().SetMatrix(renderer.GetWorldToScreenMatrix());
	transformSystem.DebugRender(renderer.GetWorldToScreenMatrix());
#endif // _DEBUG
	renderer.Present();
}

void Game::RegisterAllEntities()
{
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	entities.DeleteAllEntities();
	auto view = registry.view<const GUIDComponent>();
	for (auto entity : view) {
		const auto& guid = view.get<GUIDComponent>(entity);
		entities.AddEntity(guid.id, entity);
	}
}
