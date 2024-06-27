#pragma once
#include <string>

#include "Core/Guid.h"
#include "Reflection/Reflection.h"
#include "Reflection/Serialize.h"

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
		ROSE_DESER(GUIDComponent);
	}

	void Serialize(ryml::NodeRef node) override
	{
		ROSE_SER(GUIDComponent);
	}

	ROSE_EXPOSE_VARS(GUIDComponent, (name)(id, InfoProps::SER)(parentId, InfoProps::SER))
};