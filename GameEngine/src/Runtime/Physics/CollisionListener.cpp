#include "Physics/CollisionListener.h"

#include <box2d/box2d.h>
#include <entt/entt.hpp>

#include "Core/Entity.h"

#include "Events/EntityEventSystem.h"

#include "Core/Systems.h"

void ContactListener::BeginContact(b2Contact* contact) {
	auto entityA = entt::entity(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	auto entityB = entt::entity(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	auto& events = GETSYSTEM(EntityEventSystem);
	if (contact->GetFixtureB()->IsSensor()) {
		events.QueueEvent(EntityEvent(entityA, "EnteringSensor"));
		events.QueueEvent(EntityEvent(entityB, "SensorEntered"));
	}
	if (contact->GetFixtureA()->IsSensor()) {
		events.QueueEvent(EntityEvent(entityB, "EnteringSensor"));
		events.QueueEvent(EntityEvent(entityA, "SensorEntered"));
	}
}

void ContactListener::EndContact(b2Contact* contact) {
	auto entityA = entt::entity(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	auto entityB = entt::entity(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	auto& events = GETSYSTEM(EntityEventSystem);
	if (contact->GetFixtureB()->IsSensor()) {
		events.QueueEvent(EntityEvent(entityA, "ExitingSensor"));
		events.QueueEvent(EntityEvent(entityB, "SensorExited"));
	}
	if (contact->GetFixtureA()->IsSensor()) {
		events.QueueEvent(EntityEvent(entityB, "ExitingSensor"));
		events.QueueEvent(EntityEvent(entityA, "SensorExited"));
	}
}
