#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

enum InputKey {
	A,
	D,
	LEFT,
	RIGHT
};
enum InputMouse {
	LEFT_BUTTON,
	MIDDLE_BUTTON
};

struct KeyData {
	bool justPressed;
	bool justReleased;
	bool isPressed;
	KeyData();
};
struct InputData {
	KeyData keys[4];
	KeyData mouse[2];
	glm::vec2 mousePosition;
};

static int keys[] = { SDL_SCANCODE_A,SDL_SCANCODE_D,SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT };
static int keyCount = 4;
static int mouseButtonCount = 2;

class InputSystem
{
	InputData input;
public:
	InputSystem();
	~InputSystem();
	void Update();
	KeyData GetKey(InputKey key);
	KeyData GetMouseButton(InputMouse mouse);
	glm::vec2 GetMousePosition();
};

