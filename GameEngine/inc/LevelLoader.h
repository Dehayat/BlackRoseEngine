#pragma once
#include <string>
#include <ryml/ryml.hpp>
#include <entt/entt.hpp>

template<typename T> class ComponentSer {
public:
	static void Deserialize(entt::registry& registry, ryml::NodeRef node, entt::entity componentOwner) {
		registry.emplace<T>(componentOwner, node);
	}
};


class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();
	void LoadLevel(const std::string& fileName);
	void SaveLevel(const std::string& fileName);
private:
	void DeserializeLevel(entt::registry& registry, ryml::NodeRef node);
	entt::entity DeserializeEntity(entt::registry& registry, ryml::NodeRef node);
	void SerializeLevel(entt::registry& registry, ryml::NodeRef node);
	void SerializeEntity(entt::registry& registry, ryml::NodeRef node, entt::entity entity);
};

