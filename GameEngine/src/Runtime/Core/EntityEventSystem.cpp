#include "Events/EntityEventSystem.h"

#include <entt/entt.hpp>

#include "Entity.h"

#include "Systems.h"

#include "Scripting/ScriptSystem.h"

#include "Components/ScriptComponent.h"

#include "Debugging/Logger.h"

void EntityEventSystem::Update()
{
	auto& registry = GETSYSTEM(Entities).GetRegistry();
	auto& scriptSystem = GETSYSTEM(ScriptSystem);

	while (!eventQueue.empty()) {
		auto& entityEvent = eventQueue.front();
		if (registry.valid(entityEvent.entity)) {
			auto scriptComponent = registry.try_get<ScriptComponent>(entityEvent.entity);
			if (scriptComponent != nullptr) {
				scriptSystem.CallEvent(entityEvent);
			}
			else {
				auto& trx = registry.get<TransformComponent>(entityEvent.entity);
				if (trx.hasParent) {
					auto parentScript = registry.try_get<ScriptComponent>(trx.parent);
					if (parentScript != nullptr) {
						auto parentEvent = EntityEvent(trx.parent, entityEvent.name);
						scriptSystem.CallEvent(parentEvent);
					}
				}
			}
		}
		eventQueue.pop();
	}
}

void EntityEventSystem::QueueEvent(EntityEvent entityEvent)
{
	eventQueue.push(entityEvent);
}
