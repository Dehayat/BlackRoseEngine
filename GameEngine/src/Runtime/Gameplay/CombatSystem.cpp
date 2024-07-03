#include "CombatSystem.h"

#include <box2d/b2_contact.h>

#include "Core/Entity.h"
#include "Core/Systems.h"
#include "Core/Log.h"

#include "Events/EventBus.h"
#include "Events/PhysicsEvent.h"
#include "Events/EntityEvent.h"
#include "Events/EntityEventSystem.h"

#include "Components/HitBoxComponent.h"
#include "Components/HurtBoxComponent.h"

CombatSystem::CombatSystem()
{
	ROSE_GETSYSTEM(EventBus).ListenToEvent<PhysicsEvent>(this, &CombatSystem::OnPhysicsEvent);
}

void CombatSystem::OnPhysicsEvent(PhysicsEvent& e)
{
	if(e.begin)
	{
		auto entityA = entt::entity(e.contact->GetFixtureA()->GetBody()->GetUserData().pointer);
		auto entityB = entt::entity(e.contact->GetFixtureB()->GetBody()->GetUserData().pointer);

		auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
		auto& events = ROSE_GETSYSTEM(EntityEventSystem);

		if(registry.any_of<HitBoxComponent>(entityA) && registry.any_of<HurtBoxComponent>(entityB))
		{
			auto& hitBox = registry.get<HitBoxComponent>(entityA);
			auto& hurtBox = registry.get<HurtBoxComponent>(entityB);
			if(hitBox.faction != hurtBox.faction)
			{
				auto entityEvent = EntityEvent(entityB, "Hit");
				events.QueueEvent(entityEvent);
			}
		}

		if(registry.any_of<HitBoxComponent>(entityB) && registry.any_of<HurtBoxComponent>(entityA))
		{
			auto& hitBox = registry.get<HitBoxComponent>(entityB);
			auto& hurtBox = registry.get<HurtBoxComponent>(entityA);
			if(hitBox.faction != hurtBox.faction)
			{
				auto entityEvent = EntityEvent(entityA, "Hit");
				events.QueueEvent(entityEvent);
			}
		}
	}
}
