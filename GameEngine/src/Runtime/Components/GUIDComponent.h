#pragma once
#include <string>

#include "Core/Guid.h"
#include "Reflection/Reflection.h"

#include "Components/Components.h"


struct GUIDComponent:IComponent
{
	Guid id;
	Guid parentId;
	std::string name;

	entt::entity parent;

	GUIDComponent()
	{
		parentId = -1;
		this->id = GuidGenerator::New();
		name = "";
		parent = NoEntity();
	}
	GUIDComponent(Guid id)
	{
		parentId = -1;
		this->id = id;
		name = "New Entity";
		parent = NoEntity();
	}
	GUIDComponent(ryml::NodeRef& node)
	{
		this->id = GuidGenerator::New();
		parentId = -1;
		name = "New Entity";
		parent = NoEntity();
		if(node.has_child("id"))
		{
			node["id"] >> id;
		}
		if(node.has_child("name"))
		{
			node["name"] >> name;
		}
		if(node.has_child("parentId"))
		{
			node["parentId"] >> parentId;
		}
	}

	void Serialize(ryml::NodeRef node) override
	{
		node |= ryml::MAP;
		node["name"] << this->name;
		node["id"] << this->id;
		node["parentId"] << this->parentId;
	}

	ROSE_EXPOSE_VARS(GUIDComponent, (name))
};