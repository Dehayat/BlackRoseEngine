#pragma once

#include "Events/EntityEvent.h"

#include "Logger.h"

class Script {
public:
	virtual void Setup() {
		//Logger::Log("Setup Script");
	}
	virtual void Update() {
		//Logger::Log("Update Script");
	}
	virtual void OnEvent(const EntityEvent& entityEvent) {
		//Logger::Log(entityEvent.name);
	}
};