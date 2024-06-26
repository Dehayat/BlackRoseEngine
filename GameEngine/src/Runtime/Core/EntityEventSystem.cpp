#include "Events/EntityEventSystem.h"

#include <entt/entt.hpp>

#include "Core/Entity.h"

#include "Core/Systems.h"

#include "Scripting/ScriptSystem.h"

#include "Components/ScriptComponent.h"
#include "Components/SendEventsToParentComponent.h"

#include "Core/Log.h"

void EntityEventSystem::Update()
{
	auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& scriptSystem = ROSE_GETSYSTEM(ScriptSystem);

	while(!eventQueue.empty())
	{
		auto& entityEvent = eventQueue.front();
		if(registry.valid(entityEvent.entity))
		{
			auto scriptComponent = registry.try_get<ScriptComponent>(entityEvent.entity);
			if(scriptComponent != nullptr)
			{
				scriptSystem.CallEvent(entityEvent);
			} else
			{
				if(registry.any_of<SendEventsToParentComponent>(entityEvent.entity))
				{
					auto& trx = registry.get<TransformComponent>(entityEvent.entity);
					if(trx.parent!=NoEntity())
					{
						auto parentScript = registry.try_get<ScriptComponent>(trx.parent);
						if(parentScript != nullptr)
						{
							entityEvent._callEventFrom = trx.parent;
							scriptSystem.CallEvent(entityEvent);
						}
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
