#pragma once
#include "Events/EventBus.h"
#include "Events/PhysicsEvent.h"

class CombatSystem
{
public:
	CombatSystem();
	void OnPhysicsEvent(PhysicsEvent& e);
};