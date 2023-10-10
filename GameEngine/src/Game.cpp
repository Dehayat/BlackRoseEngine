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

struct Sprite {
	std::string sprite;
	int layer;
	SDL_Color color;
	Sprite(std::string sprite, int layer = 0, SDL_Color color = SDL_Color{ 255,255,255,255 }) {
		this->sprite = sprite;
		this->layer = layer;
		this->color = color;
	}
};

struct Player {
	float speed;
	int input = 0;
	Player(float speed = 10) {
		this->speed = speed;
		input = 0;
	}

};

struct Camera {
	float height;
	glm::mat3 camToScreen;
	glm::mat3 worldToScreen;
	Camera(float height = 10) {
		this->height = height;
		camToScreen = glm::mat3();
		worldToScreen = glm::mat3();
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
	this->camera = camera;

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
		auto camM = registry.get<Camera>(camera);
		auto camT = registry.get<Transform>(camera);
		imgui.Render(camM.camToScreen, camM.worldToScreen, camT.matrix, camT.position);
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
				auto screenToCam = glm::inverse(registry.get<Camera>(camera).camToScreen);
				auto screenToWorld = glm::inverse(registry.get<Camera>(camera).worldToScreen);
				auto mousePos = glm::vec3(sdlEvent.button.x, sdlEvent.button.y, 1);
				auto CamPos = mousePos * screenToCam;
				auto worldPos = mousePos * screenToWorld;
				Logger::Log("Clicked on pos " + std::to_string(sdlEvent.button.x) + "," + std::to_string(sdlEvent.button.y));
				Logger::Log("Cam Pos " + std::to_string(CamPos[0]) + "," + std::to_string(CamPos[1]));
				Logger::Log("World Pos " + std::to_string(worldPos[0]) + "," + std::to_string(worldPos[1]));
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

	registry.get<Transform>(camera).position = glm::vec2(x, y);

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
	SDL_SetRenderDrawColor(sdl->GetRenderer(), 38, 77, 142, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdl->GetRenderer());
	registry.sort<Sprite>([](const auto& lhs, const auto& rhs) {
		return lhs.layer < rhs.layer;
		});

	auto camPos = registry.get<Transform>(camera);
	auto& cam = registry.get<Camera>(camera);
	auto camHeight = cam.height;
	auto camMatrix = glm::mat3(
		glm::cos(glm::radians(camPos.rotation)), -glm::sin(glm::radians(camPos.rotation)), camPos.position.x,
		glm::sin(glm::radians(camPos.rotation)), glm::cos(glm::radians(camPos.rotation)), camPos.position.y,
		0, 0, 1
	);
	//assume ok
	auto invCamMatrix = glm::inverse(camMatrix);
	auto windowSize = sdl->GetWindowSize();
	float windowAspect = (float)windowSize.x / windowSize.y;
	float camWidth = windowAspect * camHeight;
	float scaleX = windowSize.x / (camWidth);
	float scaleY = windowSize.y / (camHeight);

	auto tr = glm::mat3(
		1, 0, camWidth / 2,
		0, -1, camHeight / 2,
		0, 0, 1);
	auto sc = glm::mat3(
		scaleX, 0, 0,
		0, scaleY, 0,
		0, 0, 1);
	//assume ok
	glm::mat3 camToScreen = tr * sc;
	auto worldToScreen = invCamMatrix * camToScreen;
	cam.camToScreen = camToScreen;
	cam.worldToScreen = worldToScreen;



	auto view2 = registry.view<const Sprite, const Transform>();
	for (auto entity : view2) {
		const auto& pos = view2.get<Transform>(entity);
		const auto& sp = view2.get<Sprite>(entity);
		const auto& texture = assetStore->GetTexture(sp.sprite);
		int texW;
		int texH;
		SDL_QueryTexture(texture.texture, nullptr, nullptr, &texW, &texH);
		auto viewMatrix = pos.matrix * worldToScreen;
		auto position = glm::vec2(viewMatrix[0][2], viewMatrix[1][2]);
		auto scale = glm::vec2(glm::sqrt(viewMatrix[0][0] * viewMatrix[0][0] + viewMatrix[1][0] * viewMatrix[1][0]), glm::sqrt(viewMatrix[0][1] * viewMatrix[0][1] + viewMatrix[1][1] * viewMatrix[1][1]));
		auto rotation = glm::degrees(std::atan2f(viewMatrix[1][0], viewMatrix[0][0]));

		int spriteSizeX = scale.x * ((float)texW / texture.ppu);
		int spriteSizeY = scale.y * ((float)texH / texture.ppu);
		SDL_FRect player = SDL_FRect{
			(position.x - spriteSizeX / 2),
			(position.y - spriteSizeY / 2),
			(float)spriteSizeX,
			(float)spriteSizeY,
		};
		SDL_SetTextureColorMod(texture.texture, sp.color.r, sp.color.g, sp.color.b);
		SDL_SetTextureBlendMode(texture.texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture.texture, sp.color.a);
		SDL_RenderCopyExF(sdl->GetRenderer(), texture.texture, nullptr, &player, rotation, nullptr, SDL_FLIP_NONE);
	}

#define DEBUG_PHYSICS
#ifdef DEBUG_PHYSICS
	physics->DebugRender(worldToScreen);
	transformSystem.DebugRender(worldToScreen, registry);
#endif // DEBUG_PHYSICS

	SDL_RenderPresent(sdl->GetRenderer());
}
