#pragma once
#include <queue>

#include "Events/EntityEvent.h"

class EntityEventSystem {
private:
	std::queue<EntityEvent> eventQueue;

public:
	void Update();
	void QueueEvent(EntityEvent entityEvent);
};