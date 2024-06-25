#pragma once
#include <string>

#include "Core/Guid.h"
#include "Reflection/Reflection.h"

#include "Components/Components.h"


struct GUIDComponent :IComponent {
	Guid id;
	std::string name;
	GUIDComponent() {
		this->id = GuidGenerator::New();
		name = "";
	}
	GUIDComponent(Guid id) {
		this->id = id;
		name = "New Entity";
	}
	GUIDComponent(ryml::NodeRef& node)
	{
		this->id = GuidGenerator::New();
		name = "New Entity";
		if (node.has_child("id"))
		{
			node["id"] >> id;
		}
		if (node.has_child("name"))
		{
			node["name"] >> name;
		}
	}

	void Serialize(ryml::NodeRef node) override
	{
		node |= ryml::MAP;
		node["name"] << this->name;
		node["id"] << this->id;
	}

	ROSE_EXPOSE_VARS(GUIDComponent,(name))
};