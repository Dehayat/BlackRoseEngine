#include "EventSystem/EntityEventSystem.h"

#include "Logger.h"

void EntityEventSystem::Update()
{
	while (!eventQueue.empty()) {
		auto &entityEvent = eventQueue.front();
		Logger::Log(entityEvent.name);
		//TODO: notify things about entity events (scripts,etc..)
		eventQueue.pop();
	}
}

void EntityEventSystem::QueueEvent(EntityEvent entityEvent)
{
	eventQueue.push(entityEvent);
}
