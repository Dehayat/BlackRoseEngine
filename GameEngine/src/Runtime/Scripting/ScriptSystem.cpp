#include "Scripting/ScriptSystem.h"

#include <set>

#include "Core/Entity.h"
#include "AssetPipline/AssetStore.h"
#include "AssetPipline/ScriptAsset.h"

#include "Core/Transform.h"
#include "Core/TimeSystem.h"
#include "Core/DisableSystem.h"
#include "Core/LevelTree.h"

#include "Core/Systems.h"

#include "Components/ScriptComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/GUIDComponent.h"
#include "Components/DisableComponent.h"

ScriptSystem::ScriptSystem()
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	registry.on_construct<ScriptComponent>().connect<&ScriptSystem::ScriptComponentCreated>(this);
	registry.on_destroy<ScriptComponent>().connect<&ScriptSystem::ScriptComponentDestroyed>(this);
}

static entt::entity GetChild(entt::entity entity, std::string childName)
{
	return ROSE_GETSYSTEM(TransformSystem).GetChild(entity, childName);
}
static std::string GetEntityName(entt::entity entity)
{
	return ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<GUIDComponent>(entity).name;
}
static void Translate(entt::entity entity, glm::vec2 translation)
{
	auto& transform = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<TransformComponent>(entity);
	transform.globalPosition += translation;
	transform.UpdateLocals();
}
static void Translate(entt::entity entity, float x, float y)
{
	Translate(entity, vec2(x, y));
}
static void PlayAnimation(entt::entity entity, const std::string& animName)
{
	if(ROSE_GETSYSTEM(EntitySystem).EntityExists(entity) && ROSE_GETSYSTEM(EntitySystem).GetRegistry().any_of<AnimationComponent>(entity))
	{
		auto& anim = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<AnimationComponent>(entity);
		anim.Play(animName);
	}
}
static void FaceDir(entt::entity entity, int dir)
{
	auto& transform = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<TransformComponent>(entity);
	transform.scale.x = glm::abs(transform.scale.x) * dir;
	transform.UpdateGlobals();
}
static void DisableEntity(entt::entity entity)
{
	ROSE_GETSYSTEM(DisableSystem).Disable(entity);
}
static void EnableEntity(entt::entity entity)
{
	ROSE_GETSYSTEM(DisableSystem).Enable(entity);
}
static std::set<entt::entity> destroyCalls;
static void DestroyEntity(entt::entity entity)
{
	destroyCalls.insert(entity);
}
static entt::entity FindEntity(std::string entityName)
{
	return ROSE_GETSYSTEM(LevelTree).FindEntity(entityName);
}
static glm::vec2 GetPos(entt::entity entity)
{
	auto& transform = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<TransformComponent>(entity);
	return transform.globalPosition;
}

void ScriptSystem::ScriptComponentCreated(entt::registry& registry, entt::entity entity)
{
	if(registry.any_of<ScriptComponent>(entity))
	{
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		for(auto& script : scriptComponent.scripts)
		{
			if(script != "")
			{
				auto scriptAsset = (ScriptAsset*)ROSE_GETSYSTEM(AssetStore).GetAsset(script).asset;
				if(scriptAsset != nullptr)
				{
					AddScript(entity, script, scriptAsset->script);
				}
			}
		}
	}
	setupNextFrame.insert(entity);
}

void ScriptSystem::ScriptComponentDestroyed(entt::registry& registry, entt::entity entity)
{
	if(registry.any_of<ScriptComponent>(entity))
	{
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		scriptStates[entity].clear();
	}
}

void ScriptSystem::Update()
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	for(auto entity : setupNextFrame)
	{
		if(registry.valid(entity) && registry.any_of<ScriptComponent>(entity))
		{
			auto& scriptComponent = registry.get<ScriptComponent>(entity);
			for(auto script : scriptComponent.scripts)
			{
				auto& state = scriptStates[entity][script];
				auto setup = state["setup"];
				if(setup.valid())
				{
					setup(entity);
				}
			}
		}
	}
	setupNextFrame.clear();
	auto view = registry.view<ScriptComponent>(entt::exclude<DisableComponent>);
	auto dt = ROSE_GETSYSTEM(TimeSystem).GetdeltaTime();
	for(auto entity : view)
	{
		if(destroyCalls.find(entity) != destroyCalls.end())
		{
			continue;
		}
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		for(auto& script : scriptComponent.scripts)
		{
			if(destroyCalls.find(entity) != destroyCalls.end())
			{
				break;
			}
			auto& state = scriptStates[entity][script];
			auto update = state["update"];
			if(update.valid())
			{
				update(entity, dt);
			}
		}
	}
	for(auto entity : destroyCalls)
	{
		ROSE_GETSYSTEM(EntitySystem).DestroyEntity(entity);
	}
	destroyCalls.clear();
}

void ScriptSystem::CallEvent(EntityEvent eventData)
{
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto entity = eventData._callEventFrom;
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	for(auto& script : scriptComponent.scripts)
	{
		auto& state = scriptStates[entity][script];
		auto eventCall = state["on_event"];
		if(eventCall.valid())
		{
			eventCall(entity, eventData);
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
	state.new_usertype<EntityEvent>("EntityEvent",
		"name", &EntityEvent::name,
		"entity", &EntityEvent::entity,
		"target", &EntityEvent::target,
		"input_key", &EntityEvent::inputKey
	);
	state.new_usertype<glm::vec2>("vec2",
		"x", &glm::vec2::x,
		"y", &glm::vec2::y,
		sol::meta_function::addition,
		sol::resolve<glm::vec2(const glm::vec2&, const glm::vec2&)>(operator+),
		sol::meta_function::subtraction,
		sol::resolve<glm::vec2(const glm::vec2&, const glm::vec2&)>(operator-)
	);
	state.set_function("get_child", GetChild);
	state.set_function("move", sol::overload(
		sol::resolve<void(entt::entity, float, float)>(Translate),
		sol::resolve<void(entt::entity, glm::vec2)>(Translate)
	));
	state.set_function("face", FaceDir);
	state.set_function("play_anim", PlayAnimation);
	state.set_function("disable", DisableEntity);
	state.set_function("enable", EnableEntity);
	state.set_function("get_name", GetEntityName);
	state.set_function("destroy", DestroyEntity);
	state.set_function("find", FindEntity);
	state.set_function("get_position", GetPos);
	state["no_entity"] = NoEntity();
	state.script(script);
}

void ScriptSystem::RefreshScript(entt::entity entity)
{
	auto& states = scriptStates[entity];
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	for(auto& script : scriptComponent.scripts)
	{
		if(states.find(script) == states.end())
		{
			auto scriptAsset = (ScriptAsset*)ROSE_GETSYSTEM(AssetStore).GetAsset(script).asset;
			if(scriptAsset != nullptr)
			{
				AddScript(entity, script, scriptAsset->script);
			}
		}
	}
	setupNextFrame.insert(entity);
}

void ScriptSystem::RemoveScript(entt::entity entity, const std::string& removeScript)
{
	auto& states = scriptStates[entity];
	entt::registry& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	if(scriptComponent.scripts.find(removeScript) != scriptComponent.scripts.end())
	{
		scriptComponent.scripts.erase(removeScript);
		states.erase(removeScript);
	}
}
