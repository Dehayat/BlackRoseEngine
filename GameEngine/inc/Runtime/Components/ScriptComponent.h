#pragma once
#include "Scripting/Script.h"

#include <set>

#include "Scripting/SpawnerScript.h"

#include <ryml/ryml.hpp>

struct ScriptComponent {
	std::set<std::string> scripts;
	ScriptComponent() {
	}

	ScriptComponent(ryml::NodeRef& node)
	{
		if (node.has_child("scripts") && node["scripts"].is_seq()) {
			auto child = node["scripts"].first_child();
			for (int i = 0; i < node["scripts"].num_children(); i++) {
				std::string script;
				child >> script;
				scripts.insert(script);
				child = child.next_sibling();
			}
		}
	}

	void Serialize(ryml::NodeRef& node)
	{
		node |= ryml::MAP;
		auto child = node.append_child();
		child.set_key("scripts");
		child |= ryml::SEQ;
		for (auto& script : scripts) {
			auto scriptNode = child.append_child();
			scriptNode << script;
		}
	}
};