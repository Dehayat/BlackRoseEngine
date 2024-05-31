#pragma once

#include <entt/entt.hpp>

#include "Components/Components.h"

class IComponentEditor {
public:
	virtual void Editor(entt::entity entity) = 0;
};