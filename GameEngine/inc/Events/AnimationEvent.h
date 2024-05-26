#pragma once
#include "Events/EntityEvent.h"

#include <string>

class AnimationEvent :public EntityEvent {
public:
	AnimationEvent(entt::entity entity, const std::string& eventName) :EntityEvent(entity,eventName){}
};