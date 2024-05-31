#pragma once
#include "Scripting/Script.h"

#include <entt/entt.hpp>

#include "TimeSystem.h"
#include "Entity.h"

#include "Systems.h"

#include "Components/TransformComponent.h"
#include "Components/PhysicsBodyComponent.h"

class SpawnerScript : public Script {
private:
	float spawnDelay;
	float currentTime;
public:
	virtual void Setup() override {
		spawnDelay = 5;
		currentTime = 0;
	}
	virtual void Update() override {
		auto& timeSystem = GETSYSTEM(TimeSystem);
		currentTime += timeSystem.GetdeltaTime();
		if (currentTime > spawnDelay) {
			currentTime = 0;
			auto& entities = GETSYSTEM(Entities);
			auto entity = entities.CreateEntity();
			entities.GetRegistry().emplace<TransformComponent>(entity);
			entities.GetRegistry().emplace<PhysicsBodyComponent>(entity);
		}
	}
};