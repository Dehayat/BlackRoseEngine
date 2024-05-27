#include "LevelLoader.h"

#include <ios>
#include <sstream>
#include <ryml/ryml_std.hpp>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

#include "Systems.h"
#include "FileResource.h"

#include "Components/TransformComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/GUIDComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/ScriptComponent.h"


LevelLoader::LevelLoader()
{
}
LevelLoader::~LevelLoader()
{
}
void LevelLoader::LoadLevel(const std::string& fileName)
{
	auto& registry = GETSYSTEM(Entities).GetRegistry();
	auto fileHandle = FileResource(fileName);
	if (fileHandle.file == nullptr) {
		Logger::Error("File" + fileName + " not found");
		return;
	}
	std::string fileString = std::string("\0", SDL_RWsize(fileHandle.file));
	SDL_RWread(fileHandle.file, &fileString[0], sizeof(fileString[0]), fileString.size());
	auto tree = ryml::parse_in_arena(ryml::to_csubstr(fileString));
	auto root = tree.rootref();
	DeserializeLevel(registry, root);
}
void LevelLoader::DeserializeLevel(entt::registry& registry, ryml::NodeRef& node)
{
	auto child = node.first_child();
	for (int i = 0; i < node.num_children(); i++) {
		if (child.is_map() && child.has_child("Type")) {
			if (child["Type"] == "Entity") {
				DeserializeEntity(registry, child);
			}
		}
		child = child.next_sibling();
	}
}
entt::entity LevelLoader::DeserializeEntity(entt::registry& registry,ryml::NodeRef& node)
{
	entt::entity entity;
	auto& entities = GETSYSTEM(Entities);
	auto guid = entities.GenerateGuid();
	if (node.has_child("Guid")) {
		node["Guid"] >> guid;
		entity = entities.CreateEntity(guid);
	}
	else {
		entity = entities.CreateEntity();
	}
	Logger::Log("Entity created");
	if (node.has_child("Transform")) {
		auto trx = node["Transform"];
		ComponentSer<TransformComponent>::Deserialize(registry, trx, entity);
	}
	if (node.has_child("PhysicsBody")) {
		auto phy = node["PhysicsBody"];
		ComponentSer<PhysicsBodyComponent>::Deserialize(registry, phy, entity);
	}
	if (node.has_child("Camera")) {
		auto n = node["Camera"];
		ComponentSer<CameraComponent>::Deserialize(registry, n, entity);
	}
	if (node.has_child("Sprite")) {
		auto n = node["Sprite"];
		ComponentSer<SpriteComponent>::Deserialize(registry, n, entity);
	}
	if (node.has_child("Player")) {
		auto n = node["Player"];
		ComponentSer<PlayerComponent>::Deserialize(registry, n, entity);
	}
	if (node.has_child("Animation")) {
		auto n = node["Animation"];
		ComponentSer<AnimationComponent>::Deserialize(registry, n, entity);
	}
	if (node.has_child("Script")) {
		auto n = node["Script"];
		ComponentSer<ScriptComponent>::Deserialize(registry, n, entity);
	}
	return entity;
}
void LevelLoader::SaveLevel(const std::string& fileName)
{
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	auto fileHandle = FileResource(fileName, "w+");
	if (fileHandle.file == nullptr) {
		Logger::Error("Couldnt create file " + fileName);
		return;
	}
	auto tree = ryml::Tree();
	auto root = tree.rootref();
	SerializeLevel(registry, root);
	std::string buffer = ryml::emitrs_yaml<std::string>(tree);
	SDL_RWwrite(fileHandle.file, buffer.data(), 1, buffer.size());
}
void LevelLoader::SerializeLevel(entt::registry& registry, ryml::NodeRef& node)
{
	node |= ryml::SEQ;
	auto view = registry.view<GUIDComponent>();
	for (auto entity : view) {
		SerializeEntity(registry, node, entity);
	}
}
void LevelLoader::SerializeEntity(entt::registry& registry, ryml::NodeRef& parent, entt::entity entity)
{
	auto node = parent.append_child();
	node |= ryml::MAP;
	node["Type"] << "Entity";
	node["Guid"] << registry.get<GUIDComponent>(entity).id;

	Logger::Log("Entity created");
	if (registry.any_of<TransformComponent>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Transform");
		registry.get<TransformComponent>(entity).Serialize(componentNode);
	}
	if (registry.any_of<SpriteComponent>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Sprite");
		registry.get<SpriteComponent>(entity).Serialize(componentNode);
	}
	if (registry.any_of<CameraComponent>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Camera");
		registry.get<CameraComponent>(entity).Serialize(componentNode);
	}
	if (registry.any_of<PlayerComponent>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Player");
		registry.get<PlayerComponent>(entity).Serialize(componentNode);
	}
	if (registry.any_of<PhysicsBodyComponent>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("PhysicsBody");
		registry.get<PhysicsBodyComponent>(entity).Serialize(componentNode);
	}
	if (registry.any_of<AnimationComponent>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Animation");
		registry.get<AnimationComponent>(entity).Serialize(componentNode);
	}
}
