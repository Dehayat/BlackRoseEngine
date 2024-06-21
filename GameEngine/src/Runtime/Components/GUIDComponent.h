#pragma once
#include <string>

#include "Core/Guid.h"
#include "Reflection/Reflection.h"

#include "Components/Components.h"


struct GUIDComponent :IComponent {
	Guid id;
	std::string name;
	bool enabled;
	GUIDComponent() {
		this->id = GuidGenerator::New();
		name = "";
		enabled = true;
	}
	GUIDComponent(Guid id) {
		this->id = id;
		name = "";
		enabled = true;
	}
	GUIDComponent(ryml::NodeRef& node)
	{
		this->id = GuidGenerator::New();
		name = "";
		enabled = true;
		if (node.has_child("id"))
		{
			node["id"] >> id;
		}
		if (node.has_child("name"))
		{
			node["name"] >> name;
		}
		if (node.has_child("enabled"))
		{
			node["enabled"] >> enabled;
		}
	}

	void Serialize(ryml::NodeRef node) override
	{
		node |= ryml::MAP;
		node["name"] << this->name;
		node["id"] << this->id;
		node["enabled"] << this->enabled;
	}

	ROSE_EXPOSE_VARS(GUIDComponent,(name)(enabled))
};