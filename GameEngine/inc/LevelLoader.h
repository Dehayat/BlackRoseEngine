#pragma once
#include <string>
#include <ryml/ryml.hpp>
#include <entt/entt.hpp>

template<typename T> class ComponentSer {
public:
	static T Deserialize(entt::registry& registry, ryml::NodeRef node, entt::entity componentOwner) {
		return registry.emplace<T>(componentOwner, node);
	}
};

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();
	void LoadLevel(const std::string& fileName, entt::registry& registry);
	void DeserializeLevel(entt::registry& registry, ryml::NodeRef node);
	entt::entity DeserializeEntity(entt::registry& registry, ryml::NodeRef node);
};

