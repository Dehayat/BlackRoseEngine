#pragma once
#include <string>

#include "Core/Entity.h"
#include "Core/Reflection.h"

#include "Core/Systems.h"

#include "Components/Components.h"


struct GUIDComponent :IComponent {
	Guid id;
	std::string name;
	GUIDComponent() {
		this->id = GETSYSTEM(Entities).GenerateGuid();
		name = "";
	}
	GUIDComponent(Guid id) {
		this->id = id;
		name = "";
	}
	GUIDComponent(ryml::NodeRef& node)
	{
		this->id = GETSYSTEM(Entities).GenerateGuid();
		name = "";
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