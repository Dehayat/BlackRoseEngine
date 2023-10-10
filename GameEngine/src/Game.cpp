#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <box2d/b2_world_callbacks.h>
#include <imgui.h>
#include "Events/KeyPressedEvent.h"
#include "Game.h"
#include "Logger.h"

struct Player {
	float speed;
	int input = 0;
	Player(float speed = 10) {
		this->speed = speed;
		input = 0;
	}

};

Game::Game() {
	Logger::Log("Game Constructed");

	sdl = std::make_unique<SdlContainer>(1200, (float)1200 * 9 / 16);
	assetStore = std::make_unique<AssetStore>();
	physics = std::make_unique<Physics>(0, -10);
	physics->InitDebugDrawer(sdl->GetRenderer());
	renderer = std::make_unique<Renderer>(sdl->GetRenderer());
	transformSystem.InitDebugDrawer(sdl->GetRenderer());
	//physics->EnableDebug(true);
	//transformSystem.EnableDebug(true);

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


	const auto hornet = registry.create();
	registry.emplace<Transform>(hornet, glm::vec2(-3.5, 0), glm::vec2(1, 1), 0);
	registry.emplace<Sprite>(hornet, "hornet", 1, SDL_Color{ 255,255,255,255 });
	registry.emplace<Player>(hornet, 10);
	registry.emplace<PhysicsBody>(hornet, *physics, glm::vec2(0, 0), glm::vec2(0.2, 0.8), true);
	player = hornet;

	const auto camera = registry.create();
	registry.emplace<Transform>(camera, glm::vec2(0, 0), glm::vec2(1, 1), 0);
	registry.emplace<Camera>(camera, 10);
	renderer->SetCamera(camera);

	const auto ground = registry.create();
	registry.emplace<Transform>(ground, glm::vec2(0, -3), glm::vec2(20, 1), 0);
	registry.emplace<Sprite>(ground, "block", 0, SDL_Color{ 255,255,255,255 });
	registry.emplace<StaticBody>(ground, *physics, glm::vec2(0, -3), glm::vec2(10, 0.4));

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
	if (sdl->ProcessEvents(imgui)) {
		isRunning = false;
	}
	input.Update(sdl->GetWindow());
	transformSystem.Update(registry);
	physics->Update(registry);

	if (input.GetKey(InputKey::LEFT).isPressed || input.GetKey(InputKey::A).isPressed) {
		registry.get<Player>(player).input = -1;
	}
	else if (input.GetKey(InputKey::RIGHT).isPressed || input.GetKey(InputKey::D).isPressed) {
		registry.get<Player>(player).input = 1;
	}
	else {
		registry.get<Player>(player).input = 0;
	}
	if (input.GetMouseButton(InputMouse::LEFT_BUTTON).justPressed) {
		const auto ground = registry.create();
		auto spawnPos = glm::vec3(input.GetMousePosition(), 1) * renderer->GetScreenToWorldMatrix();
		registry.emplace<Transform>(ground, glm::vec2(spawnPos.x, spawnPos.y), glm::vec2(1, 1), 0);
		registry.emplace<Sprite>(ground, "rose", 0, SDL_Color{ 255,255,255,255 });
		registry.emplace<PhysicsBody>(ground, *physics, glm::vec2(spawnPos.x, spawnPos.y), glm::vec2(0.25, 0.25));
	}
	auto view2 = registry.view<const Player, Transform>();
	for (auto entity : view2) {
		auto& pos = view2.get<Transform>(entity);
		const auto& player = view2.get<Player>(entity);
		pos.position.x += player.speed * player.input * dt;
	}

	int waitTimeMs = FRAMETIME_MS - (SDL_GetTicks() - msLastFrame);
	if (waitTimeMs > 0 && waitTimeMs < FRAMETIME_MS)
		SDL_Delay(waitTimeMs);
	dt = (SDL_GetTicks() - msLastFrame) / 1000.0f;
	msLastFrame = SDL_GetTicks();
}
float camX, camY;
void ImguiCam(Transform& tf) {
	ImGui::SliderFloat("CamX", &camX, -10, 10);
	ImGui::SliderFloat("CamY", &camY, -10, 10);
	tf.position.x = camX;
	tf.position.y = camY;
}
void Game::Render()
{

	renderer->Render(&registry, *assetStore);
#define DEBUG_PHYSICS
#ifdef DEBUG_PHYSICS
	physics->DebugRender(renderer->GetWorldToScreenMatrix());
	transformSystem.DebugRender(renderer->GetWorldToScreenMatrix(), registry);
#endif // DEBUG_PHYSICS
	renderer->Present();

	imgui.Render();
	ImguiCam(registry.get<Transform>(renderer->GetCamera()));
	imgui.Present();

}
