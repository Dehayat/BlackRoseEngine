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

public:
	virtual void Setup() override {
	}
	virtual void Update() override {
		auto& timeSystem = GETSYSTEM(TimeSystem);
		float dt = timeSystem.GetdeltaTime();
	}
	virtual void OnEvent(const EntityEvent& entityEvent) override {
		if (entityEvent.name == "LeftKeyPressed") {
			auto& transform = GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(entityEvent.entity);
			transform.position.x -= 0.25;
		}
		if (entityEvent.name == "RightKeyPressed") {
			auto& transform = GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(entityEvent.entity);
			transform.position.x += 0.25;
		}
	}
};