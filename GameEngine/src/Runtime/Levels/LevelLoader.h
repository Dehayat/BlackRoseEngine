#pragma once
#include <string>

#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>
#include <entt/entt.hpp>


class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();
	void LoadLevel(const std::string& fileName);
	void SaveLevel(const std::string& fileName);
	void UnloadLevel();
	const std::string& GetCurrentLevelFile();
private:
	std::string loadedLevel;
	void DeserializeLevel(entt::registry& registry, ryml::NodeRef& node);
	entt::entity DeserializeEntity(entt::registry& registry, ryml::NodeRef& node);
	void SerializeLevel(entt::registry& registry, ryml::NodeRef& node);
	void SerializeEntity(entt::registry& registry, ryml::NodeRef& node, entt::entity entity);
	template<typename TComponent>
	void SerializeComponent(entt::registry& registry, const char* name, entt::entity entity, c4::yml::NodeRef& node)
	{
		if(registry.any_of<TComponent>(entity))
		{
			auto componentNode = node.append_child();
			componentNode.set_key(c4::to_csubstr(name));
			registry.get<TComponent>(entity).Serialize(componentNode);
		}
	}

};

