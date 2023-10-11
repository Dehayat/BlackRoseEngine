#pragma once
#include <string>
#include <ryml/ryml.hpp>
#include <entt/entt.hpp>
#include "Transform.h"
#include "Physics.h"

template<typename T> class ComponentSer {
public:
	virtual T Deserialize(entt::registry& registry, ryml::NodeRef node, entt::entity componentOwner) {
		return registry.emplace<T>(componentOwner, node);
	}
};

class LevelLoader
{
	ComponentSer<Transform> transform;
	ComponentSer<PhysicsBody> physicsBody;
public:
	LevelLoader();
	~LevelLoader();
	void LoadLevel(const std::string& fileName, entt::registry& registry);
	void DeserializeLevel(entt::registry& registry, ryml::NodeRef node);
	entt::entity DeserializeEntity(entt::registry& registry, ryml::NodeRef node);
};

