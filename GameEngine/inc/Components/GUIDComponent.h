#pragma once

#include "Entity.h"

#include "Systems.h"


struct GUIDComponent {
	Guid id;
	GUIDComponent() {
		this->id = GETSYSTEM(Entities).GenerateGuid();
	}
	GUIDComponent(Guid id) {
		this->id = id;
	}
};