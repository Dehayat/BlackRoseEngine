#pragma once
#include "Reflection/Serialize.h"

struct HitBoxComponent
{
	int faction;
	HitBoxComponent()
	{
		faction = 0;
	}

	HitBoxComponent(ryml::NodeRef node)
	{
		faction = 0;
		ROSE_DESER(HitBoxComponent);
	}
	void Serialize(ryml::NodeRef node)
	{
		ROSE_SER(HitBoxComponent);
	}

	ROSE_EXPOSE_VARS(HitBoxComponent,(faction))
};