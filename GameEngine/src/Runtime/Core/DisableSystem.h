#pragma once
#include <unordered_map>

#include "Entity.h"

enum class stateChange {
	NOTHING,
	JUST_ENABLED,
	JUST_DISABLED,
};

class DisableSystem {
	std::unordered_map<entt::entity, bool> isEnabled;
	std::unordered_map<entt::entity, stateChange> enableChange;
public:
	void Update();
	bool IsEnabled(entt::entity entity);
	bool JustEnabled(entt::entity entity);
	bool JustDisabled(entt::entity entity);
};