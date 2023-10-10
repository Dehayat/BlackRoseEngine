#include "SdlContainer.h"

SdlContainer::SdlContainer(int windowWidth, int windowHeight)
{
	auto res = SDL_Init(SDL_INIT_EVERYTHING);
	SDL_assert(res == 0);

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
	SDL_assert(window != nullptr);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_assert(renderer != nullptr);
	SDL_RenderSetVSync(renderer, 1);
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
	int x, y;
	SDL_GetWindowSize(window, &x, &y);
	return glm::ivec2(x, y);
}
