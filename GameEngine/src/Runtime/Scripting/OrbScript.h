#pragma once
#include "Scripting/Script.h"

#include <entt/entt.hpp>

#include "Core/TimeSystem.h"
#include "Core/Entity.h"

#include "Core/Systems.h"

#include "Components/TransformComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/AnimationComponent.h"

class OrbScript : public Script {
public:
	virtual void OnEvent(entt::entity owner, const EntityEvent& entityEvent) override {

		if (entityEvent.name == "SensorEntered") {
			auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
			if (registry.all_of<AnimationComponent>(owner)) {
				auto& anim = registry.get<AnimationComponent>(owner);
				anim.Play("OrbExplodeAnim");
			}
		}
	}
};