#pragma once
#include "Scripting/Script.h"

#include "Scripting/SpawnerScript.h"
#include "Scripting/PlayerScript.h"

#include <ryml/ryml.hpp>

struct ScriptComponent {
	Script* script;
	ScriptComponent(Script* script) {
		this->script = script;
	}
	ScriptComponent() {
		this->script = nullptr;
		script = new PlayerScript();
	}
	~ScriptComponent() {
		if (script != nullptr) {
			delete script;
		}
	}
	ScriptComponent(const ScriptComponent&) = delete;
	ScriptComponent& operator=(const ScriptComponent&) = delete;

	ScriptComponent(ryml::NodeRef& node)
	{
		//script = new Script();
		//script = new SpawnerScript();
		script = new PlayerScript();
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
	void Serialize(ryml::NodeRef& node)
	{
		node |= ryml::MAP;
	}
};