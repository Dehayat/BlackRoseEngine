#pragma once
#include <sdl2/SDL_keycode.h>
#include "EventSystem/Event.h"

class KeyPressedEvent :public Event {
public:
	SDL_Keycode key;
	KeyPressedEvent(SDL_Keycode key) {
		this->key = key;
	}
};