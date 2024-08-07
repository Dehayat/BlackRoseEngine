#include "LevelLoader.h"

#include <ios>
#include <sstream>

#include <SDL2/SDL.h>
#include <entt/entt.hpp>

#include "Core/Systems.h"
#include "Core/Guid.h"

#include "Core/FileResource.h"

#include "Components/TransformComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/GUIDComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/ScriptComponent.h"
#include "Components/SendEventsToParentComponent.h"
#include "Components/DisableComponent.h"
#include "Components/InputComponent.h"
#include "Components/HitBoxComponent.h"
#include "Components/HurtBoxComponent.h"

#include "Core/Log.h"


LevelLoader::LevelLoader()
{
}
LevelLoader::~LevelLoader()
{
}
void LevelLoader::LoadLevel(const std::string& fileName)
{
	auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
	auto fileHandle = FileResource(fileName);
	if(fileHandle.file == nullptr)
	{
		ROSE_ERR("File %s not found", fileName.c_str());
		return;
	}
	std::string fileString = std::string("\0", SDL_RWsize(fileHandle.file));
	SDL_RWread(fileHandle.file, &fileString[0], sizeof(fileString[0]), fileString.size());
	auto tree = ryml::parse_in_arena(ryml::to_csubstr(fileString));
	auto root = tree.rootref();
	DeserializeLevel(registry, root);
	loadedLevel = fileName;
}
void LevelLoader::DeserializeLevel(entt::registry& registry, ryml::NodeRef& node)
{
	auto child = node.first_child();
	for(int i = 0; i < node.num_children(); i++)
	{
		if(child.is_map() && child.has_child("Type"))
		{
			if(child["Type"] == "Entity")
			{
				DeserializeEntity(registry, child);
			}
		}
		child = child.next_sibling();
	}
}
entt::entity LevelLoader::DeserializeEntity(entt::registry& registry, ryml::NodeRef& node)
{
	return ROSE_GETSYSTEM(EntitySystem).DeserializeEntity(node);
}
void LevelLoader::SerializeEntity(entt::registry& registry, ryml::NodeRef& parent, entt::entity entity)
{
	auto node = parent.append_child();
	node |= ryml::MAP;
	node["Type"] << "Entity";

	SerializeComponent<GUIDComponent>(registry, "Guid", entity, node);
	SerializeComponent<DisableComponent>(registry, "Disabled", entity, node);
	SerializeComponent<TransformComponent>(registry, "Transform", entity, node);
	SerializeComponent<SpriteComponent>(registry, "Sprite", entity, node);
	SerializeComponent<CameraComponent>(registry, "Camera", entity, node);
	SerializeComponent<PhysicsBodyComponent>(registry, "PhysicsBody", entity, node);
	SerializeComponent<AnimationComponent>(registry, "Animation", entity, node);
	SerializeComponent<ScriptComponent>(registry, "Script", entity, node);
	SerializeComponent<SendEventsToParentComponent>(registry, "SendEventsToParent", entity, node);
	SerializeComponent<InputComponent>(registry, "Input", entity, node);
	SerializeComponent<HitBoxComponent>(registry, "HitBox", entity, node);
	SerializeComponent<HurtBoxComponent>(registry, "HurtBox", entity, node);
}

void LevelLoader::SaveLevel(const std::string& fileName)
{
	EntitySystem& entities = entt::locator<EntitySystem>::value();
	entt::registry& registry = entities.GetRegistry();
	auto fileHandle = FileResource(fileName, "w+");
	if(fileHandle.file == nullptr)
	{
		ROSE_ERR("Couldnt create file %s", fileName.c_str());
		return;
	}
	auto tree = ryml::Tree();
	auto root = tree.rootref();
	SerializeLevel(registry, root);
	std::string buffer = ryml::emitrs_yaml<std::string>(tree);
	SDL_RWwrite(fileHandle.file, buffer.data(), 1, buffer.size());
}
void LevelLoader::UnloadLevel()
{
	EntitySystem& entities = ROSE_GETSYSTEM(EntitySystem);
	entities.DestroyAllEntities();
}
const std::string& LevelLoader::GetCurrentLevelFile()
{
	return loadedLevel;
}
void LevelLoader::SerializeLevel(entt::registry& registry, ryml::NodeRef& node)
{
	node |= ryml::SEQ;
	registry.sort<TransformComponent>([](const auto& lhs, const auto& rhs)
		{
			return lhs.level < rhs.level;
		});
	auto view = registry.view<GUIDComponent, TransformComponent>().use<TransformComponent>();
	for(auto entity : view)
	{
		auto& trx = view.get<TransformComponent>(entity);
		SerializeEntity(registry, node, entity);
	}
}