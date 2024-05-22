#include "SdlContainer.h"

SdlContainer::SdlContainer(int windowWidth, int windowHeight)
{
	auto res = SDL_Init(SDL_INIT_EVERYTHING);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	window = SDL_CreateWindow(
		"Rose Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
	);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

SdlContainer::~SdlContainer()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

SDL_Renderer* SdlContainer::GetRenderer()
{
	return renderer;
}

SDL_Window* SdlContainer::GetWindow()
{
	return window;
}

glm::ivec2 SdlContainer::GetWindowSize()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	return glm::ivec2(width, height);
}
bool SdlContainer::ProcessEvents()
{
	bool exit = false;
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			exit = true;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				exit = true;
			}
			break;
		default:
			break;
		}
	}
	return exit;
}
