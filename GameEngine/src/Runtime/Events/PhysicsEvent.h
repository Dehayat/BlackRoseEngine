#pragma once
#include "Event.h"
#include <box2d/b2_world_callbacks.h>

class PhysicsEvent:public Event
{
public:
	b2Contact* contact;
	bool begin;
	PhysicsEvent(b2Contact* contact, bool begin):contact(contact), begin(begin)
	{

	}
};