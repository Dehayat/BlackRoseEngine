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
	Q,
	W,
	E,
	C,
	M,
	S,
	X,
	O,
	F2,
	LSHIFT,
	RSHIFT,

	COUNT
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
	SDL_SCANCODE_Q,
	SDL_SCANCODE_W,
	SDL_SCANCODE_E,
	SDL_SCANCODE_C,
	SDL_SCANCODE_M,
	SDL_SCANCODE_S,
	SDL_SCANCODE_X,
	SDL_SCANCODE_O,
	SDL_SCANCODE_F2,
	SDL_SCANCODE_LSHIFT,
	SDL_SCANCODE_RSHIFT,
};
const int keyCount = InputKey::COUNT;

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

