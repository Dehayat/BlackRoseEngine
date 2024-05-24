#pragma once
#include "EventSystem/Event.h"

#include <string>

class AnimationEvent :public Event {
public:
	std::string name;
	AnimationEvent(const std::string& name) {
		this->name = name;
	}
};