#pragma once

#include "Events/EntityEvent.h"

#include "Debug/Logger.h"

class Script {
public:
	virtual void Setup(entt::entity owner) {
		//Logger::Log("Setup Script");
	}
	virtual void Update(entt::entity owner) {
		//Logger::Log("Update Script");
	}
	virtual void OnEvent(entt::entity owner, const EntityEvent& entityEvent) {
		//Logger::Log(entityEvent.name);
	}
};