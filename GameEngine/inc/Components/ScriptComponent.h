#pragma once
#include "ScriptSystem/Script.h"

#include "Scripts/SpawnerScript.h"

#include <ryml/ryml.hpp>

struct ScriptComponent {
	Script* script;
	ScriptComponent(Script* script) {
		this->script = script;
	}
	~ScriptComponent() {
		if (script != nullptr) {
			delete script;
		}
	}
	ScriptComponent(const ScriptComponent&) = delete;
	ScriptComponent& operator=(const ScriptComponent&) = delete;

	ScriptComponent(ryml::NodeRef node)
	{
		//script = new Script();
		script = new SpawnerScript();
		Logger::Log("deser");
	}
	ScriptComponent(ScriptComponent&& other) {
		script = other.script;
		other.script = nullptr;
	}

	ScriptComponent& operator=(ScriptComponent&& other) {
		Script* temp = other.script;
		other.script = nullptr;
		if (script != nullptr) {
			delete script;
		}
		script = temp;
		return *this;
	}

};