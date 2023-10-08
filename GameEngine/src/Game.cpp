#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <fstream>
#include "Logger.h"
#include <Events/KeyPressedEvent.h>

Game::Game() {
	Logger::Log("Game Constructed");
	auto res = SDL_Init(SDL_INIT_EVERYTHING);
	SDL_assert(res==0);
		 
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

void Game::Setup()
{
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

	SDL_Rect player = SDL_Rect{
		10,
		10,
		20,
		20
	};
	SDL_SetRenderDrawColor(renderer, 120, 120, 120, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &player);

	SDL_Surface* surface = IMG_Load("./assets/Rose.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

	SDL_RenderPresent(renderer);
}

