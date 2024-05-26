#pragma once
#include <string>

#include "EventSystem/Event.h"
#include "entt/entity/entity.hpp"

struct EntityEvent :public Event {
	const entt::entity entity;
	const std::string name;

	EntityEvent(entt::entity entity, const std::string& eventName) :entity(entity), name(eventName) {}
};