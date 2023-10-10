#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <box2d/b2_world_callbacks.h>
#include "Events/KeyPressedEvent.h"
#include "Game.h"

Game::Game() {
	Logger::Log("Game Constructed");

	sdl = std::make_unique<SdlContainer>(1200, (float)1200 * 9 / 16);
	assetStore = std::make_unique<AssetStore>();
	physics = std::make_unique<Physics>(0, -10);
	physics->InitDebugDrawer(sdl->GetRenderer());
	renderer = std::make_unique<Renderer>(sdl->GetRenderer());
	physics->EnableDebug(true);
	transformSystem.InitDebugDrawer(sdl->GetRenderer());
	transformSystem.EnableDebug(true);

	dt = 0;
	msLastFrame = 0;
	isRunning = false;
}

Game::~Game() {
	Logger::Log("Game Destructed");
}


struct Player {
	float speed;
	int input = 0;
	Player(float speed = 10) {
		this->speed = speed;
		input = 0;
	}

};


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
		ProcessEvents();
		Update();
		Render();
		imgui.Render();
	}
}

void Game::ProcessEvents()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		if (sdlEvent.window.windowID != SDL_GetWindowID(sdl->GetWindow())) {
			imgui.HandleEvent(sdlEvent);
			continue;
		}
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (sdlEvent.button.button == 1) {
				auto screenToWorld = glm::inverse(renderer->GetWorldToScreenMatrix());
				auto mousePos = glm::vec3(sdlEvent.button.x, sdlEvent.button.y, 1);
				auto worldPos = mousePos * screenToWorld;
				auto rose = registry.create();
				registry.emplace<Transform>(rose, worldPos, glm::vec2(1, 1), 0);
				registry.emplace<Sprite>(rose, "rose", 5);
				registry.emplace<PhysicsBody>(rose, *physics, glm::vec2(worldPos.x, worldPos.y), glm::vec2(0.25, 0.25), true);

			}
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
			break;
		default:
			break;
		}
	}

}

static int keys[] = { SDL_SCANCODE_A,SDL_SCANCODE_D,SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT };
static int keyCount = 4;

void Game::UpdateInputSystem() {
	//int* keyboardState = nullptr;
	auto keyArray = SDL_GetKeyboardState(nullptr);
	for (int i = 0;i < keyCount;i++) {
		if (keyArray[keys[i]] == 1) {
			if (input.keys[i].isPressed) {
				input.keys[i].justPressed = false;
			}
			else {
				input.keys[i].isPressed = true;
				input.keys[i].justPressed = true;
				input.keys[i].justReleased = false;
			}
		}
		else {
			if (input.keys[i].isPressed) {
				input.keys[i].isPressed = false;
				input.keys[i].justReleased = true;
				input.keys[i].justPressed = false;
			}
			else {
				input.keys[i].justReleased = false;
			}
		}
	}
}

float x, y;

void Game::Update()
{
	physics->Update(registry);
	transformSystem.Update(registry);

	if (SDL_GetKeyboardFocus() == sdl->GetWindow()) {
		UpdateInputSystem();
	}

	registry.get<Transform>(renderer->GetCamera()).position = glm::vec2(x, y);

	auto view2 = registry.view<const Player, Transform>();

	if (input.keys[0].isPressed || input.keys[2].isPressed) {
		registry.get<Player>(player).input = -1;
	}
	else if (input.keys[1].isPressed || input.keys[3].isPressed) {
		registry.get<Player>(player).input = 1;
	}
	else {
		registry.get<Player>(player).input = 0;
	}

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


void Game::Render()
{

	renderer->Render(&registry, *assetStore);
#define DEBUG_PHYSICS
#ifdef DEBUG_PHYSICS
	physics->DebugRender(renderer->GetWorldToScreenMatrix());
	transformSystem.DebugRender(renderer->GetWorldToScreenMatrix(), registry);
#endif // DEBUG_PHYSICS
	renderer->Present();

}
