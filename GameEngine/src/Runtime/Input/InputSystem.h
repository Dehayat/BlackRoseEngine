#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

enum InputKey {
	A,
	D,
	LEFT,
	RIGHT,
	DELETE,
	LCTRL,
	RCTRL,
	N,
};

static int keys[] = {
	SDL_SCANCODE_A,
	SDL_SCANCODE_D,
	SDL_SCANCODE_LEFT,
	SDL_SCANCODE_RIGHT,
	SDL_SCANCODE_DELETE,
	SDL_SCANCODE_LCTRL,
	SDL_SCANCODE_RCTRL,
	SDL_SCANCODE_N,
};
const int keyCount = 8;

enum InputMouse {
	LEFT_BUTTON,
	MIDDLE_BUTTON
};
const int mouseButtonCount = 2;

struct KeyData {
	bool justPressed;
	bool justReleased;
	bool isPressed;
	KeyData();
};
struct InputData {
	KeyData keys[keyCount];
	KeyData mouse[mouseButtonCount];
	glm::vec2 mousePosition;
};

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

