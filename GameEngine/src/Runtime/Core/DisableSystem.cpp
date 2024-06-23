#include "DisableSystem.h"

#include <entt/entt.hpp>

#include "Core/Systems.h"
#include "Core/Log.h"

#include "Components/GUIDComponent.h"
#include "Components/TransformComponent.h"

void DisableSystem::Update()
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();

	registry.sort<TransformComponent>([](const auto& lhs, const auto& rhs) {
		return lhs.level < rhs.level;
		});
	auto view3 = registry.view<GUIDComponent, TransformComponent>();
	for (auto entity : view3) {
		bool wasEnabled = IsEnabled(entity);
		auto& guid = view3.get<GUIDComponent>(entity);
		auto& trx = view3.get<TransformComponent>(entity);
		if (guid.enabled) {
			isEnabled[entity] = true;
		}
		else {
			isEnabled[entity] = false;
		}
		if (trx.hasParent) {
			if (isEnabled[trx.parent] == false) {
				isEnabled[entity] = false;
			}
		}
		bool isNowEnabled = IsEnabled(entity);
		if (isNowEnabled == wasEnabled) {
			enableChange[entity] = stateChange::NOTHING;
		}
		else {
			if (isNowEnabled) {

				enableChange[entity] = stateChange::JUST_ENABLED;
			}
			else {
				enableChange[entity] = stateChange::JUST_DISABLED;
			}
		}
	}
}

bool DisableSystem::IsEnabled(entt::entity entity)
{
	if (isEnabled.find(entity) != isEnabled.end()) {
		return isEnabled[entity];
	}
	return false;
}

bool DisableSystem::JustEnabled(entt::entity entity)
{
	if (enableChange.find(entity) != enableChange.end()) {
		return enableChange[entity] == stateChange::JUST_ENABLED;
	}
	return false;
}

bool DisableSystem::JustDisabled(entt::entity entity)
{
	if (enableChange.find(entity) != enableChange.end()) {
		return enableChange[entity] == stateChange::JUST_DISABLED;
	}
	return false;
}