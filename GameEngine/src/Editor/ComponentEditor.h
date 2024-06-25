#pragma once

#include <entt/entity/entity.hpp>

class IComponentEditor {
public:
	virtual void Editor(entt::entity entity) = 0;
};