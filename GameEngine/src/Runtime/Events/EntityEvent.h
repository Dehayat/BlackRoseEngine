#pragma once
#include <string>

#include "Events/Event.h"
#include <Core/Entity.h>


struct EntityEvent: public Event
{
	entt::entity _callEventFrom;
	const entt::entity entity;
	const std::string name;

	entt::entity target;

	EntityEvent(entt::entity entity, const std::string& eventName):entity(entity), name(eventName)
	{
		_callEventFrom = entity;
		target = NoEntity();
	}
};