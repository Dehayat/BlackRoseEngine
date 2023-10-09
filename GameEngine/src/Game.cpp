#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include "Events/KeyPressedEvent.h"
#include "Game.h"
#include "Logger.h"

Game::Game() {
	Logger::Log("Game Constructed");
	auto res = SDL_Init(SDL_INIT_EVERYTHING);
	SDL_assert(res == 0);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = 1200; //displayMode.w;
	windowHeight = (float)windowWidth * 9 / 16;// displayMode.h;
	window = SDL_CreateWindow(
		"Rose Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN
	);
	SDL_assert(window != nullptr);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_assert(renderer != nullptr);
	SDL_RenderSetVSync(renderer, 1);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	eventBus = std::make_unique<EventBus>();

	assetStore = std::make_unique<AssetStore>();

	dt = 0;

	msLastFrame = 0;

	isRunning = false;
}

Game::~Game() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	Logger::Log("Game Destructed");
}

struct Transform {
	glm::vec2 position;
	glm::vec2 scale;
	float rotation;
	glm::mat3 matrix;
	Transform(glm::vec2 position = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1, 1), float rotation = 0) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
		this->matrix = glm::mat3(0);
	}
};

struct Sprite {
	std::string sprite;
	int layer;
	SDL_Color color;
	float ppu;
	glm::vec2 size;
	Sprite(std::string sprite, int layer = 0, SDL_Color color = SDL_Color{ 255,255,255,255 }, float ppu = 100) {
		this->sprite = sprite;
		this->size = size;
		this->layer = layer;
		this->color = color;
		this->ppu = ppu;
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
	Camera(float height = 10) {
		this->height = height;
	}
};

void Game::Setup()
{
	assetStore->AddTexture(renderer, "rose", "./assets/Rose.png");
	assetStore->AddTexture(renderer, "hornet", "./assets/Hornet_Idle.png");


	const auto hornet = registry.create();
	registry.emplace<Transform>(hornet, glm::vec2(0, 0), glm::vec2(1, 1), 0);
	registry.emplace<Sprite>(hornet, "hornet", 1);
	registry.emplace<Player>(hornet, 10);
	player = hornet;

	const auto camera = registry.create();
	registry.emplace<Transform>(camera, glm::vec2(0, 0), glm::vec2(1, 1), 0);
	registry.emplace<Camera>(camera, 10);
	this->camera = camera;

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
		imgui.HandleEvent(sdlEvent);
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			isRunning = false;
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

void Game::Update()
{
	UpdateInputSystem();

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
	auto view3 = registry.view<Transform>();
	for (auto entity : view3) {
		auto& pos = view3.get<Transform>(entity);
		//calc matrix
		pos.matrix = glm::mat3(
			glm::cos(glm::radians(pos.rotation)) * pos.scale.x, -glm::sin(glm::radians(pos.rotation)) * pos.scale.y, pos.position.x,
			glm::sin(glm::radians(pos.rotation)) * pos.scale.x, glm::cos(glm::radians(pos.rotation)) * pos.scale.y, pos.position.y,
			0, 0, 1
		);
	}


	eventBus->Reset();
}


void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 38, 77, 142, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	registry.sort<Sprite>([](const auto& lhs, const auto& rhs) {
		return lhs.layer > rhs.layer;
		});

	auto camPos = registry.get<Transform>(camera);
	auto camHeight = registry.get<Camera>(camera).height;
	auto camMatrix = glm::mat3(
		glm::cos(glm::radians(camPos.rotation)), -glm::sin(glm::radians(camPos.rotation)), camPos.position.x,
		glm::sin(glm::radians(camPos.rotation)), glm::cos(glm::radians(camPos.rotation)), camPos.position.y,
		0, 0, 1
	);
	auto invCamMatrix = glm::inverse(camMatrix);
	float windowAspect = (float)windowWidth / windowHeight;
	float camWidth = windowAspect * camHeight;
	float scaleX = windowWidth / (camWidth);
	float scaleY = windowHeight / (camHeight);

	auto tr = glm::mat3(
		1, 0, camWidth / 2,
		0, -1, camHeight / 2,
		0, 0, 1);
	auto sc = glm::mat3(
		scaleX, 0, 0,
		0, scaleY, 0,
		0, 0, 1);

	glm::mat3 camToScreen = tr * sc;
	auto worldToScreen = camToScreen * invCamMatrix;



	auto view2 = registry.view<const Sprite, const Transform>();
	for (auto entity : view2) {
		const auto& pos = view2.get<Transform>(entity);
		const auto& sp = view2.get<Sprite>(entity);
		const auto& texture = assetStore->GetTexture(sp.sprite);
		int texW;
		int texH;
		SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
		auto viewMatrix = pos.matrix * worldToScreen;
		auto position = glm::vec2(viewMatrix[0][2], viewMatrix[1][2]);
		auto scale = glm::vec2(glm::sqrt(viewMatrix[0][0] * viewMatrix[0][0] + viewMatrix[1][0] * viewMatrix[1][0]), glm::sqrt(viewMatrix[0][1] * viewMatrix[0][1] + viewMatrix[1][1] * viewMatrix[1][1]));
		auto rotation = glm::degrees(std::atan2f(viewMatrix[1][0], viewMatrix[0][0]));

		int spriteSizeX = scale.x * (texW / sp.ppu);
		int spriteSizeY = scale.y * (texH / sp.ppu);
		SDL_Rect player = SDL_Rect{
			(int)(position.x - spriteSizeX / 2),
			(int)(position.y - spriteSizeY / 2),
			spriteSizeX,
			spriteSizeY,
		};
		SDL_SetTextureColorMod(texture, sp.color.r, sp.color.g, sp.color.b);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture, sp.color.a);
		SDL_RenderCopyEx(renderer, texture, nullptr, &player, rotation, nullptr, SDL_FLIP_NONE);
	}

	SDL_RenderPresent(renderer);
}

