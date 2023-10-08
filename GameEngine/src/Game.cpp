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
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;
	window = SDL_CreateWindow(
		"Rose Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS
	);
	SDL_assert(window != nullptr);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_assert(renderer != nullptr);

	eventBus = std::make_unique<EventBus>();

	assetStore = std::make_unique<AssetStore>();

	camera = { 0,0,windowWidth,windowHeight };

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
public:
	Transform(glm::vec2 position = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1, 1), float rotation = 0) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
	}
};

struct Sprite {
	std::string sprite;
	int layer;
	SDL_Color color;
	glm::vec2 size;
public:
	Sprite(std::string sprite, glm::vec2 size = glm::vec2(100, 100), int layer = 0, SDL_Color color = SDL_Color{ 255,255,255,255 }) {
		this->sprite = sprite;
		this->size = size;
		this->layer = layer;
		this->color = color;
	}
};

void Game::Setup()
{
	assetStore->AddTexture(renderer, "rose", "./assets/Rose.png");
	assetStore->AddTexture(renderer, "hornet", "./assets/Hornet_Idle.png");


	const auto hornet = registry.create();
	registry.emplace<Transform>(hornet, glm::vec2(500, 500), glm::vec2(1, 1));
	registry.emplace<Sprite>(hornet, "hornet", glm::vec2(256, 256), 1);

}

void Game::Run()
{
	Setup();
	isRunning = true;
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
			else {
				eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
			}
			break;
		default:
			break;
		}
	}

}

void Game::Update()
{

	int waitTimeMs = FRAMETIME_MS - (SDL_GetTicks() - msLastFrame);
	if (waitTimeMs > 0 && waitTimeMs < FRAMETIME_MS)
		SDL_Delay(waitTimeMs);
	dt = (SDL_GetTicks() - msLastFrame) / 1000.0f;
	msLastFrame = SDL_GetTicks();

	eventBus->Reset();
}


void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 80, 10, 50, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	registry.sort<Sprite>([](const auto& lhs, const auto& rhs) {
		return lhs.layer > rhs.layer;
		});
	auto view2 = registry.view<const Sprite, const Transform>();
	for (auto entity : view2) {
		const auto& pos = view2.get<Transform>(entity);
		const auto& sp = view2.get<Sprite>(entity);
		SDL_Rect player = SDL_Rect{
			(int)pos.position.x,
			(int)pos.position.y,
			(int)(pos.scale.x*sp.size.x),
			(int)(pos.scale.y*sp.size.y)
		};
		SDL_SetTextureColorMod(assetStore->GetTexture(sp.sprite), sp.color.r, sp.color.g, sp.color.b);
		SDL_SetTextureBlendMode(assetStore->GetTexture(sp.sprite), SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(assetStore->GetTexture(sp.sprite), sp.color.a);
		SDL_RenderCopyEx(renderer, assetStore->GetTexture(sp.sprite), nullptr, &player, pos.rotation, nullptr, SDL_FLIP_NONE);
	}

	SDL_RenderPresent(renderer);
}

