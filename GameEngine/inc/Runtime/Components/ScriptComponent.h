#pragma once
#include "Scripting/Script.h"

#include "Scripting/SpawnerScript.h"

#include <ryml/ryml.hpp>

struct ScriptComponent {
	std::string script;
	ScriptComponent() {
		script = "playerScript";
	}

	ScriptComponent(ryml::NodeRef& node)
	{
		script = "";
		if (node.has_child("script")) {
			node["script"] >> script;
		}
	}

	void Serialize(ryml::NodeRef& node)
	{
		node |= ryml::MAP;
		node["script"] << script;
	}
};