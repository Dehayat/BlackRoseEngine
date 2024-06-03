#pragma once
#include "Scripting/Script.h"

#include <entt/entt.hpp>

#include "TimeSystem.h"
#include "Entity.h"

#include "Systems.h"

#include "Components/TransformComponent.h"
#include "Components/PhysicsBodyComponent.h"

class PlayerScript : public Script {
private:
	int walkDir = 0;
	int speed = 5;
public:
	virtual void Update(entt::entity owner) override {
		auto& timeSystem = GETSYSTEM(TimeSystem);
		float dt = timeSystem.GetdeltaTime();
		if (walkDir != 0) {
			auto& transform = GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(owner);
			transform.position.x += walkDir * dt * speed;
		}
	}
	virtual void OnEvent(entt::entity owner, const EntityEvent& entityEvent) override {
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
	}
};