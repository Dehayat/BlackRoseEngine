#if _WIN64
#define SDL_MAIN_HANDLED
#endif

#include "Application.h"
#ifdef _EDITOR
#include "Editor/Editor.h"
#else
#include "Runtime/Core/Game.h"
#endif // _EDITOR


int main(int argc, char* argv[])
{
	IApplication* app = nullptr;

#ifdef _EDITOR
	app = new Editor();
#else
	app = new Game();
#endif

	app->Run();
	return 0;
}