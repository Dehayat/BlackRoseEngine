#include "InputSystem.h"

#include "Systems.h"
#include "SdlContainer.h"

KeyData::KeyData() {
	justPressed = false;
	justReleased = false;
	isPressed = false;
}

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::Update() {
	SDL_Window*  window = GETSYSTEM(SdlContainer).GetWindow();
	if (SDL_GetKeyboardFocus() != window) {
		return;
	}
	auto keyArray = SDL_GetKeyboardState(nullptr);
	for (int i = 0;i < keyCount;i++) {
		if (keyArray[keys[i]] == 1) {
			if (input.keys[i].isPressed) {
				input.keys[i].justPressed = false;
			}
			else {
				input.keys[i].isPressed = true;
				input.keys[i].justPressed = true;
				input.keys[i].justReleased = false;
			}
		}
		else {
			if (input.keys[i].isPressed) {
				input.keys[i].isPressed = false;
				input.keys[i].justReleased = true;
				input.keys[i].justPressed = false;
			}
			else {
				input.keys[i].justReleased = false;
			}
		}
	}
	auto mousePos = glm::ivec2(0, 0);
	int buttonMask = SDL_GetMouseState(&(mousePos.x), &(mousePos.y));
	if (buttonMask & SDL_BUTTON(1)) {
		if (input.mouse[0].isPressed) {
			input.mouse[0].justPressed = false;
		}
		else {
			input.mouse[0].isPressed = true;
			input.mouse[0].justPressed = true;
			input.mouse[0].justReleased = false;
		}
	}
	else {
		if (input.mouse[0].isPressed) {
			input.mouse[0].isPressed = false;
			input.mouse[0].justReleased = true;
			input.mouse[0].justPressed = false;
		}
		else {
			input.mouse[0].justReleased = false;
		}
	}

	if (buttonMask & SDL_BUTTON(2)) {
		if (input.mouse[1].isPressed) {
			input.mouse[1].justPressed = false;
		}
		else {
			input.mouse[1].isPressed = true;
			input.mouse[1].justPressed = true;
			input.mouse[1].justReleased = false;
		}
	}
	else {
		if (input.mouse[1].isPressed) {
			input.mouse[1].isPressed = false;
			input.mouse[1].justReleased = true;
			input.mouse[1].justPressed = false;
		}
		else {
			input.mouse[1].justReleased = false;
		}
	}
	input.mousePosition = mousePos;
}

KeyData InputSystem::GetKey(InputKey key)
{
	SDL_assert((key >= 0 && key < keyCount));
	return input.keys[key];
}

KeyData InputSystem::GetMouseButton(InputMouse mouse)
{
	SDL_assert((mouse >= 0 && mouse < mouseButtonCount));
	return input.mouse[mouse];
}

glm::vec2 InputSystem::GetMousePosition()
{
	return input.mousePosition;
}
