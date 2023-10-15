#include "LevelLoader.h"
#include <ios>
#include <sstream>
#include <ryml/ryml_std.hpp>
#include <SDL2/SDL.h>
#include "Components/TransformComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/GUIDComponent.h"
#include "Player.h"

struct FileResource {
	SDL_RWops* file;
	FileResource(const std::string& fileName, const std::string how = "r") {
		file = SDL_RWFromFile(fileName.c_str(), how.c_str());
	}
	~FileResource() {
		if (file != nullptr) {
			file->close(file);
		}
	}
};

LevelLoader::LevelLoader()
{
}
LevelLoader::~LevelLoader()
{
}
void LevelLoader::LoadLevel(const std::string& fileName, entt::registry& registry) {
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
void LevelLoader::DeserializeLevel(entt::registry& registry, ryml::NodeRef node)
{
	auto child = node.first_child();
	for (int i = 0;i < node.num_children();i++) {
		if (child.is_map() && child.has_child("Type")) {
			if (child["Type"] == "Entity") {
				DeserializeEntity(registry, child);
			}
		}
		child = child.next_sibling();
	}
}
entt::entity LevelLoader::DeserializeEntity(entt::registry& registry, ryml::NodeRef node)
{
	auto entity = registry.create();
	auto guid = GUID::Generate();
	if (node.has_child("Guid")) {
		node["Guid"] >> guid;
	}
	registry.emplace<GUID>(entity, guid);
	Logger::Log("Entity created");
	if (node.has_child("Transform")) {
		auto trx = node["Transform"];
		ComponentSer<Transform>::Deserialize(registry, trx, entity);
	}
	if (node.has_child("PhysicsBody")) {
		auto phy = node["PhysicsBody"];
		ComponentSer<PhysicsBody>::Deserialize(registry, phy, entity);
	}
	if (node.has_child("Camera")) {
		auto n = node["Camera"];
		ComponentSer<Camera>::Deserialize(registry, n, entity);
	}
	if (node.has_child("Sprite")) {
		auto n = node["Sprite"];
		ComponentSer<Sprite>::Deserialize(registry, n, entity);
	}
	if (node.has_child("Player")) {
		auto n = node["Player"];
		ComponentSer<Player>::Deserialize(registry, n, entity);
	}
	return entity;
}
void LevelLoader::SaveLevel(const std::string& fileName, entt::registry& registry)
{
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
void LevelLoader::SerializeLevel(entt::registry& registry, ryml::NodeRef node)
{
	node |= ryml::SEQ;
	auto view = registry.view<GUID>();
	for (auto entity : view) {
		SerializeEntity(registry, node, entity);
	}
}
void LevelLoader::SerializeEntity(entt::registry& registry, ryml::NodeRef parent, entt::entity entity)
{
	auto node = parent.append_child();
	node |= ryml::MAP;
	node["Type"] << "Entity";
	node["Guid"] << registry.get<GUID>(entity).id;

	Logger::Log("Entity created");
	if (registry.any_of<Transform>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Transform");
		registry.get<Transform>(entity).Serialize(componentNode);
	}
	if (registry.any_of<Sprite>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Sprite");
		registry.get<Sprite>(entity).Serialize(componentNode);
	}
	if (registry.any_of<Camera>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Camera");
		registry.get<Camera>(entity).Serialize(componentNode);
	}
	if (registry.any_of<Player>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("Player");
		registry.get<Player>(entity).Serialize(componentNode);
	}
	if (registry.any_of<PhysicsBody>(entity)) {
		auto componentNode = node.append_child();
		componentNode.set_key("PhysicsBody");
		registry.get<PhysicsBody>(entity).Serialize(componentNode);
	}
}
