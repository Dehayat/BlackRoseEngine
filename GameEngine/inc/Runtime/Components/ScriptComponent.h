#pragma once
#include "Scripting/Script.h"

#include "Scripting/SpawnerScript.h"
#include "Scripting/PlayerScript.h"

#include <ryml/ryml.hpp>

struct ScriptComponent {
	std::string scriptFile;
	ScriptComponent() {
		scriptFile = "assets/Scripts/Player.lua";
	}

	ScriptComponent(ryml::NodeRef& node)
	{
		scriptFile = "assets/Scripts/Player.lua";
	}

	void Serialize(ryml::NodeRef& node)
	{
		node |= ryml::MAP;
	}
};