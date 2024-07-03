#pragma once
#include "Reflection/Serialize.h"

struct HurtBoxComponent
{
	int faction;
	HurtBoxComponent()
	{
		faction = 0;
	}

	HurtBoxComponent(ryml::NodeRef node)
	{
		faction = 0;
		ROSE_DESER(HurtBoxComponent);
	}
	void Serialize(ryml::NodeRef node)
	{
		ROSE_SER(HurtBoxComponent);
	}

	ROSE_EXPOSE_VARS(HurtBoxComponent, (faction))
};