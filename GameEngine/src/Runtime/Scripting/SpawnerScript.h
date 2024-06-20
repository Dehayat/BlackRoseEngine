#pragma once
#include "Scripting/Script.h"

#include <entt/entt.hpp>

#include "Core/TimeSystem.h"
#include "Core/Entity.h"

#include "Core/Systems.h"

#include "Components/TransformComponent.h"
#include "Components/PhysicsBodyComponent.h"

class SpawnerScript : public Script {
private:
	float spawnDelay;
	float currentTime;
public:
	virtual void Setup(entt::entity owner) override {
		spawnDelay = 5;
		currentTime = 0;
	}
	virtual void Update(entt::entity owner) override {
		auto& timeSystem = ROSE_GETSYSTEM(TimeSystem);
		currentTime += timeSystem.GetdeltaTime();
		if (currentTime > spawnDelay) {
			currentTime = 0;
			auto& entities = ROSE_GETSYSTEM(Entities);
			auto entity = entities.CreateEntity();
			entities.GetRegistry().emplace<TransformComponent>(entity);
			entities.GetRegistry().emplace<PhysicsBodyComponent>(entity);
		}
	}
};