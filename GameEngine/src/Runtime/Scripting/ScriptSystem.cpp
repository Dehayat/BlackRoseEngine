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
	scriptStates[entity] = sol::state();
	auto& state = scriptStates[entity];
	state.open_libraries();
	state.new_usertype<entt::entity>("entity");
	state.set_function("get_child", GetChild);
	state.set_function("move", Translate);
	state.set_function("face", FaceDir);
	state.set_function("play_anim", PlayAnimation);
	state["no_entity"] = NoEntity();
	if (registry.valid(entity) && registry.any_of<ScriptComponent>(entity)) {
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		for (auto script : scriptComponent.scripts) {
			if (script != "") {
				auto scriptAsset = GETSYSTEM(AssetStore).GetAsset(script);
				if (scriptAsset.asset != nullptr) {
					auto script = (ScriptAsset*)scriptAsset.asset;
					state.script(script->script);
				}
			}
		}
	}
	setupNextFrame.push(entity);
}

void ScriptSystem::Update()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	while (!setupNextFrame.empty()) {
		auto entity = setupNextFrame.front();
		if (registry.valid(entity) && registry.any_of<ScriptComponent>(entity)) {
			auto& scriptComponent = registry.get<ScriptComponent>(entity);
			auto& state = scriptStates[entity];
			state["setup"](entity);
		}
		setupNextFrame.pop();
	}
	auto view = registry.view<ScriptComponent>();
	auto dt = GETSYSTEM(TimeSystem).GetdeltaTime();
	for (auto entity : view) {
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		auto& state = scriptStates[entity];
		sol::optional update = state["update"];
		if (update) {
			update.value()(entity, dt);
		}
	}
}

void ScriptSystem::CallEvent(EntityEvent eventData)
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	auto entity = eventData.entity;
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	auto& state = scriptStates[entity];
	sol::optional eventCall = state["on_event"];
	if (eventCall) {
		eventCall.value()(entity, eventData.name);
	}
}
