#pragma once

#include "Events/EntityEvent.h"

#include "Core/Log.h"

class Script {
public:
	virtual void Setup(entt::entity owner) {
		//ROSE_LOG("Setup Script");
	}
	virtual void Update(entt::entity owner) {
		//ROSE_LOG("Update Script");
	}
	virtual void OnEvent(entt::entity owner, const EntityEvent& entityEvent) {
		//ROSE_LOG(entityEvent.name);
	}
};