#include <sstream>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <box2d/b2_world_callbacks.h>
#include <ryml/ryml.hpp>
#include "Game.h"
#include "GUID.h"
#include "Player.h"
#include "Logger.h"

#ifdef _EDITOR
#include <imgui.h>
#endif // _EDITOR



Game::Game() :allEntities() {
	Logger::Log("Game Constructed");

	sdl = std::make_unique<SdlContainer>(1200, (float)1200 * 9 / 16);
	assetStore = std::make_unique<AssetStore>();
	physics = std::make_unique<Physics>(0, -10);
	physics->InitDebugDrawer(sdl->GetRenderer());
	renderer = std::make_unique<Renderer>(sdl->GetRenderer());
	transformSystem.InitDebugDrawer(sdl->GetRenderer());
#ifdef _DEBUG
	physics->EnableDebug(true);
	transformSystem.EnableDebug(true);
#endif // !_DEBUG

	auto x = SDL_GL_GetCurrentContext();

	dt = 0;
	msLastFrame = 0;
	isRunning = false;
}
Game::~Game() {
	Logger::Log("Game Destructed");
}
void Game::Setup()
{
	assetStore->AddTexture(sdl->GetRenderer(), "rose", "./assets/Rose.png", 512);
	assetStore->AddTexture(sdl->GetRenderer(), "hornet", "./assets/Hornet_Idle.png", 128);
	assetStore->AddTexture(sdl->GetRenderer(), "block", "./assets/Block.jpg", 64);
	assetStore->AddTexture(sdl->GetRenderer(), "platform", "./assets/Ground.png", 128);
	assetStore->AddTexture(sdl->GetRenderer(), "bg", "./assets/bg.jpg", 64);
	assetStore->AddTexture(sdl->GetRenderer(), "drip", "./assets/Drip.png", 128);

	levelLoader.LoadLevel("level.yaml", registry);

	RegisterAllEntities();
	transformSystem.InitLoaded(registry, allEntities);
	physics->InitLoaded(registry);
	renderer->InitLoaded(registry);
}
void Game::Run()
{
	Setup();
	isRunning = true;
	while (isRunning) {
		Update();
		Render();

		std::uint64_t waitTimeMs = SDL_GetTicks64() + FRAMETIME_MS - (SDL_GetTicks64() - msLastFrame);
		//while (SDL_GetTicks64() < waitTimeMs);
		dt = (SDL_GetTicks64() - msLastFrame) / 1000.0f;
		msLastFrame = SDL_GetTicks64();
	}
}
void Game::Update()
{
	if (sdl->ProcessEvents()) {
		isRunning = false;
	}

	transformSystem.Update(registry);

#ifndef _EDITOR
	input.Update(sdl->GetWindow());
	physics->Update(registry);

	if (input.GetMouseButton(InputMouse::LEFT_BUTTON).justPressed) {
		const auto ground = registry.create();
		auto spawnPos = glm::vec3(input.GetMousePosition(), 1) * renderer->GetScreenToWorldMatrix();
		registry.emplace<Transform>(ground, glm::vec2(spawnPos.x, spawnPos.y), glm::vec2(1, 1), 0);
		registry.emplace<Sprite>(ground, "rose", 0, SDL_Color{ 255,255,255,255 });
		registry.emplace<PhysicsBody>(ground, *physics, glm::vec2(spawnPos.x, spawnPos.y), glm::vec2(0.25, 0.25));
	}
	auto view2 = registry.view<Player, const Transform, PhysicsBody>();
	for (auto entity : view2) {
		const auto& pos = view2.get<Transform>(entity);
		auto& phys = view2.get<PhysicsBody>(entity);
		auto& player = view2.get<Player>(entity);
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
#endif // _EDITOR
}

void Game::Render()
{

	renderer->Render(&registry, *assetStore);
#ifdef _DEBUG
	physics->DebugRender(renderer->GetWorldToScreenMatrix());
	transformSystem.DebugRender(renderer->GetWorldToScreenMatrix(), registry);
#endif // _DEBUG
	renderer->Present();

#ifdef _EDITOR
	imgui.Render();
	//Imgui Code
	imgui.Present();
#endif // _EDITOR

}

void Game::RegisterAllEntities()
{
	allEntities.clear();
	auto view = registry.view<const GUID>();
	for (auto entity : view) {
		const auto& guid = view.get<GUID>(entity);
		allEntities.emplace(guid.id, entity);
	}
}
