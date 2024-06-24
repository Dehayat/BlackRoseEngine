#pragma once
#include <ryml/ryml.hpp>

#include "Reflection/Reflection.h"

struct DisableComponent
{
	bool selfDisabled;
	bool parentDisabled;

	DisableComponent(bool self = true, bool parent = false):selfDisabled(self), parentDisabled(parent)
	{
	}

	DisableComponent(ryml::NodeRef node)
	{
		selfDisabled = false;
		parentDisabled = false;

		if(node.is_map())
		{
			if(node.has_child("selfDisabled"))
			{
				node["selfDisabled"] >> this->selfDisabled;
			}
			if(node.has_child("parentDisabled"))
			{
				node["parentDisabled"] >> this->parentDisabled;
			}
		}
	}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		if(selfDisabled)
		{
			node["selfDisabled"] << selfDisabled;
		}
		if(parentDisabled)
		{
			node["parentDisabled"] << parentDisabled;
		}
	}

	ROSE_EXPOSE_VARS(DisableComponent, (selfDisabled)(parentDisabled))
};