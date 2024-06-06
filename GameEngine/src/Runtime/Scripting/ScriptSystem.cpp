#include "Scripting/ScriptSystem.h"

#include "Entity.h"

#include "Systems.h"

#include "Components/ScriptComponent.h"

ScriptSystem::ScriptSystem()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	registry.on_construct<ScriptComponent>().connect<&ScriptSystem::ScriptComponentCreated>(this);
}

void ScriptSystem::ScriptComponentCreated(entt::registry& registry, entt::entity entity)
{
	newlyAddedScripts.push(entity);
}

void ScriptSystem::Update()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	while (!newlyAddedScripts.empty()) {
		auto& entity = newlyAddedScripts.front();
		if (registry.valid(entity) && registry.any_of<ScriptComponent>(entity)) {
			auto& scriptComponent = registry.get<ScriptComponent>(entity);
			if (scriptComponent.script != nullptr) {
				scriptComponent.script->Setup(entity);
			}
		}
		newlyAddedScripts.pop();
	}
	auto view = registry.view<ScriptComponent>();
	for (auto entity : view) {
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		if (scriptComponent.script != nullptr) {
			scriptComponent.script->Update(entity);
		}
	}
}
