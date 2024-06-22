#pragma once
#include <ryml/ryml.hpp>

#include "Reflection/Reflection.h"

class SendEventsToParentComponent
{
	int8_t _empty;
public:
	SendEventsToParentComponent()
	{
		_empty = 0;
	}

	SendEventsToParentComponent(ryml::NodeRef node)
	{
		_empty = 0;
	}

	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::SEQ;
	}

	ROSE_EXPOSE_VARS(SendEventsToParentComponent)
};