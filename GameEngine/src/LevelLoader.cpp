#include "LevelLoader.h"
#include <ios>
#include <sstream>
#include <ryml/ryml_std.hpp>
#include <SDL2/SDL.h>
#include "Logger.h"

struct FileResource {
	SDL_RWops* file;
	FileResource(const std::string& fileName) {
		file = SDL_RWFromFile(fileName.c_str(), "r");
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
	Logger::Log("Entity created");
	if (node.has_child("Transform")) {
		auto trx = node["Transform"];
		transform.Deserialize(registry, trx, entity);
	}
	if (node.has_child("PhysicsBody")) {
		auto phy = node["PhysicsBody"];
		physicsBody.Deserialize(registry, phy, entity);
	}
	return entity;
}