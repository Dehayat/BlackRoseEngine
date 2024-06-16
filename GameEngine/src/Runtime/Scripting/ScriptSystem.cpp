#include "Scripting/ScriptSystem.h"

#include "Entity.h"
#include "AssetStore/AssetStore.h"
#include "AssetStore/ScriptAsset.h"

#include "Transform.h"
#include "TimeSystem.h"

#include "Systems.h"

#include "Components/ScriptComponent.h"
#include "Components/AnimationComponent.h"

ScriptSystem::ScriptSystem()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	registry.on_construct<ScriptComponent>().connect<&ScriptSystem::ScriptComponentCreated>(this);
}

static entt::entity GetChild(entt::entity entity, std::string childName) {
	return GETSYSTEM(TransformSystem).GetChild(entity, childName);
}
static void Translate(entt::entity entity, float x, float y) {
	auto& transform = GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(entity);
	transform.globalPosition += glm::vec2(x, y);
	transform.UpdateLocals();
}
static void PlayAnimation(entt::entity entity, const std::string& animName) {
	auto& anim = GETSYSTEM(Entities).GetRegistry().get<AnimationComponent>(entity);
	anim.Play(animName);
}
static void FaceDir(entt::entity entity, int dir) {
	auto& transform = GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(entity);
	transform.scale.x = glm::abs(transform.scale.x) * dir;
	transform.UpdateGlobals();
}

void ScriptSystem::ScriptComponentCreated(entt::registry& registry, entt::entity entity)
{
	if (registry.valid(entity) && registry.any_of<ScriptComponent>(entity)) {
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		for (auto& script : scriptComponent.scripts) {
			if (script != "") {
				auto scriptAsset = (ScriptAsset*)GETSYSTEM(AssetStore).GetAsset(script).asset;
				if (scriptAsset != nullptr) {
					AddScript(entity, script, scriptAsset->script);
				}
			}
		}
	}
	setupNextFrame.insert(entity);
}

void ScriptSystem::Update()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	for (auto& entity : setupNextFrame) {
		if (registry.valid(entity) && registry.any_of<ScriptComponent>(entity)) {
			auto& scriptComponent = registry.get<ScriptComponent>(entity);
			for (auto script : scriptComponent.scripts) {
				auto& state = scriptStates[entity][script];
				state["setup"](entity);
			}
		}
	}
	setupNextFrame.clear();
	auto view = registry.view<ScriptComponent>();
	auto dt = GETSYSTEM(TimeSystem).GetdeltaTime();
	for (auto entity : view) {
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		for (auto script : scriptComponent.scripts) {
			auto& state = scriptStates[entity][script];
			sol::optional update = state["update"];
			if (update) {
				update.value()(entity, dt);
			}
		}
	}
}

void ScriptSystem::CallEvent(EntityEvent eventData)
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	auto entity = eventData.entity;
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	for (auto script : scriptComponent.scripts) {
		auto& state = scriptStates[entity][script];
		sol::optional eventCall = state["on_event"];
		if (eventCall) {
			eventCall.value()(entity, eventData.name);
		}
	}
}

void ScriptSystem::AddScript(entt::entity entity, const std::string scriptName, const std::string script)
{
	scriptStates[entity];
	scriptStates[entity][scriptName] = sol::state();
	auto& state = scriptStates[entity][scriptName];
	state.open_libraries();
	state.new_usertype<entt::entity>("entity");
	state.set_function("get_child", GetChild);
	state.set_function("move", Translate);
	state.set_function("face", FaceDir);
	state.set_function("play_anim", PlayAnimation);
	state["no_entity"] = NoEntity();
	state.script(script);

}

void ScriptSystem::RefreshScript(entt::entity entity)
{
	auto& states = scriptStates[entity];
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	for (auto& script : scriptComponent.scripts) {
		if (states.find(script) == states.end()) {
			auto scriptAsset = (ScriptAsset*)GETSYSTEM(AssetStore).GetAsset(script).asset;
			if (scriptAsset != nullptr) {
				AddScript(entity, script, scriptAsset->script);
			}
		}
	}
	setupNextFrame.insert(entity);
}

void ScriptSystem::RemoveScript(entt::entity entity, const std::string& removeScript)
{
	auto& states = scriptStates[entity];
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	if (scriptComponent.scripts.find(removeScript) != scriptComponent.scripts.end()) {
		scriptComponent.scripts.erase(removeScript);
		states.erase(removeScript);
	}
}
