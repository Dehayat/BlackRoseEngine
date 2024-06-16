#pragma once
#include <queue>

#include "Events/EventBus.h"
#include "Events/EntityEvent.h"

class EntityEventSystem {
private:
	EventBus eventBus;
	std::queue<EntityEvent> eventQueue;

public:
	void Update();
	void QueueEvent(EntityEvent entityEvent);
};