#include "Physics/CollisionListener.h"

#include <box2d/box2d.h>
#include <entt/entt.hpp>

#include "Core/Entity.h"

#include "Events/EntityEventSystem.h"

#include "Core/Systems.h"

void ContactListener::BeginContact(b2Contact* contact) {
	auto entityA = entt::entity(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	auto entityB = entt::entity(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	auto& events = ROSE_GETSYSTEM(EntityEventSystem);
	if (contact->GetFixtureB()->IsSensor()) {
		auto entityEvent = EntityEvent(entityA, "EnteringSensor");
		entityEvent.target = entityB;
		events.QueueEvent(entityEvent);
		auto entityEvent2 = EntityEvent(entityB, "SensorEntered");
		entityEvent2.target = entityA;
		events.QueueEvent(entityEvent2);
	}
	if (contact->GetFixtureA()->IsSensor()) {
		auto entityEvent = EntityEvent(entityB, "EnteringSensor");
		entityEvent.target = entityA;
		events.QueueEvent(entityEvent);
		auto entityEvent2 = EntityEvent(entityA, "SensorEntered");
		entityEvent2.target = entityB;
		events.QueueEvent(entityEvent2);
	}
}

void ContactListener::EndContact(b2Contact* contact) {
	auto entityA = entt::entity(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	auto entityB = entt::entity(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	auto& events = ROSE_GETSYSTEM(EntityEventSystem);
	if(contact->GetFixtureB()->IsSensor())
	{
		auto entityEvent = EntityEvent(entityA, "ExitingSensor");
		entityEvent.target = entityB;
		events.QueueEvent(entityEvent);
		auto entityEvent2 = EntityEvent(entityB, "SensorExited");
		entityEvent2.target = entityA;
		events.QueueEvent(entityEvent2);
	}
	if(contact->GetFixtureA()->IsSensor())
	{
		auto entityEvent = EntityEvent(entityB, "ExitingSensor");
		entityEvent.target = entityA;
		events.QueueEvent(entityEvent);
		auto entityEvent2 = EntityEvent(entityA, "SensorExited");
		entityEvent2.target = entityB;
		events.QueueEvent(entityEvent2);
	}
}
