#include "SdlContainer.h"
#include "Logger.h"

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
	//auto context = SDL_GL_CreateContext(window);
	int a;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &a);
	Logger::Log(std::to_string(a));

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);// | SDL_RENDERER_PRESENTVSYNC);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &a);
	Logger::Log(std::to_string(a));
	SDL_assert(renderer != nullptr);
#ifndef _EDITOR
	SDL_RenderSetVSync(renderer, 1);
#endif // !_EDITOR

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
		case SDL_MOUSEBUTTONDOWN:
			//if (sdlEvent.button.button == 1) {
			//	auto screenToWorld = glm::inverse(renderer->GetWorldToScreenMatrix());
			//	auto mousePos = glm::vec3(sdlEvent.button.x, sdlEvent.button.y, 1);
			//	auto worldPos = mousePos * screenToWorld;
			//	auto rose = registry.create();
			//	registry.emplace<Transform>(rose, worldPos, glm::vec2(1, 1), 0);
			//	registry.emplace<Sprite>(rose, "rose", 5);
			//	registry.emplace<PhysicsBody>(rose, *physics, glm::vec2(worldPos.x, worldPos.y), glm::vec2(0.25, 0.25), true);
			//}
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
