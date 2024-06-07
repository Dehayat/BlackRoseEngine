#pragma once
#include "Scripting/Script.h"

#include <entt/entt.hpp>

#include "TimeSystem.h"
#include "Entity.h"

#include "Systems.h"

#include "Components/TransformComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/AnimationComponent.h"

class PlayerScript : public Script {
private:
	int walkDir = 0;
public:
	virtual void Update(entt::entity owner) override {
		auto& timeSystem = GETSYSTEM(TimeSystem);
		float dt = timeSystem.GetdeltaTime();
		if (walkDir != 0) {
			auto& transform = GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(owner);
			auto& player = GETSYSTEM(Entities).GetRegistry().get<PlayerComponent>(owner);
			transform.position.x += walkDir * dt * player.speed;
		}
	}
	virtual void OnEvent(entt::entity owner, const EntityEvent& entityEvent) override {
		auto& registry = GETSYSTEM(Entities).GetRegistry();
		int oldWalkDir = walkDir;
		if (entityEvent.name == "LeftKeyPressed") {
			walkDir = -1;
		}
		if (entityEvent.name == "LeftKeyReleased") {
			if (walkDir == -1) {
				walkDir = 0;
			}
		}
		if (entityEvent.name == "RightKeyPressed") {
			walkDir = 1;
		}
		if (entityEvent.name == "RightKeyReleased") {
			if (walkDir == 1) {
				walkDir = 0;
			}
		}
		if (oldWalkDir != walkDir) {
			if (registry.all_of<AnimationComponent, PlayerComponent>(owner)) {
				auto& anim = registry.get<AnimationComponent>(owner);
				auto& player = registry.get<PlayerComponent>(owner);
				if (walkDir == 0) {
					anim.Play(player.idleAnim);
				}
				else {
					anim.Play(player.runAnim);
				}
			}
		}
	}
};