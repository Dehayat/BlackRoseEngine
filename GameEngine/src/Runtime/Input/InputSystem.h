#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "InputKeys.h"

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

