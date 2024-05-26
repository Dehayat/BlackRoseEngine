#pragma once
#include "ScriptSystem/Script.h"

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

	ScriptComponent(ryml::NodeRef node)
	{
		script = new Script();
	}
};