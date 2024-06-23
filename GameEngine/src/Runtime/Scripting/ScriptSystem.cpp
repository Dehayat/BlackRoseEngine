#include "Scripting/ScriptSystem.h"

#include <set>

#include "Core/Entity.h"
#include "AssetPipline/AssetStore.h"
#include "AssetPipline/ScriptAsset.h"

#include "Core/Transform.h"
#include "Core/TimeSystem.h"
#include "Core/DisableSystem.h"

#include "Core/Systems.h"

#include "Components/ScriptComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/GUIDComponent.h"

ScriptSystem::ScriptSystem()
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	registry.on_construct<ScriptComponent>().connect<&ScriptSystem::ScriptComponentCreated>(this);
	registry.on_destroy<ScriptComponent>().connect<&ScriptSystem::ScriptComponentDestroyed>(this);
}

static entt::entity GetChild(entt::entity entity, std::string childName)
{
	return ROSE_GETSYSTEM(TransformSystem).GetChild(entity, childName);
}
static std::string GetEntityName(entt::entity entity)
{
	return ROSE_GETSYSTEM(Entities).GetRegistry().get<GUIDComponent>(entity).name;
}
static void Translate(entt::entity entity, float x, float y)
{
	auto& transform = ROSE_GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(entity);
	transform.globalPosition += glm::vec2(x, y);
	transform.UpdateLocals();
}
static void PlayAnimation(entt::entity entity, const std::string& animName)
{
	if(ROSE_GETSYSTEM(Entities).EntityExists(entity) && ROSE_GETSYSTEM(Entities).GetRegistry().any_of<AnimationComponent>(entity))
	{
		auto& anim = ROSE_GETSYSTEM(Entities).GetRegistry().get<AnimationComponent>(entity);
		anim.Play(animName);
	}
}
static void FaceDir(entt::entity entity, int dir)
{
	auto& transform = ROSE_GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(entity);
	transform.scale.x = glm::abs(transform.scale.x) * dir;
	transform.UpdateGlobals();
}
static void DisableEntity(entt::entity entity)
{
	auto& guid = ROSE_GETSYSTEM(Entities).GetRegistry().get<GUIDComponent>(entity);
	guid.enabled = false;
}
static void EnableEntity(entt::entity entity)
{
	auto& guid = ROSE_GETSYSTEM(Entities).GetRegistry().get<GUIDComponent>(entity);
	guid.enabled = true;
}
static std::set<entt::entity> destroyCalls;
static void DestroyEntity(entt::entity entity)
{
	destroyCalls.insert(entity);
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
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
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
	auto view = registry.view<ScriptComponent>();
	auto dt = ROSE_GETSYSTEM(TimeSystem).GetdeltaTime();
	for(auto entity : view)
	{
		if(destroyCalls.find(entity) != destroyCalls.end())
		{
			continue;
		}
		if(ROSE_GETSYSTEM(DisableSystem).IsDisabled(entity))
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
		ROSE_GETSYSTEM(Entities).DestroyEntity(entity);
	}
	destroyCalls.clear();
}

void ScriptSystem::CallEvent(EntityEvent eventData)
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
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
		"target", &EntityEvent::target
	);
	state.set_function("get_child", GetChild);
	state.set_function("move", Translate);
	state.set_function("face", FaceDir);
	state.set_function("play_anim", PlayAnimation);
	state.set_function("disable", DisableEntity);
	state.set_function("enable", EnableEntity);
	state.set_function("get_name", GetEntityName);
	state.set_function("destroy", DestroyEntity);
	state["no_entity"] = NoEntity();
	state.script(script);

}

void ScriptSystem::RefreshScript(entt::entity entity)
{
	auto& states = scriptStates[entity];
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
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
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	if(scriptComponent.scripts.find(removeScript) != scriptComponent.scripts.end())
	{
		scriptComponent.scripts.erase(removeScript);
		states.erase(removeScript);
	}
}
