#include <ryml/ryml.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

template<typename T> class ComponentSer
{
public:
	static void Deserialize(entt::registry& registry, ryml::NodeRef& node, entt::entity componentOwner)
	{
		registry.emplace<T>(componentOwner, node);
	}
};


class EntitySerializer
{
	template<typename TComponent>
	static void DeserializeComponent(entt::registry& registry, const char* name, entt::entity entity, c4::yml::NodeRef& node)
	{
		if(node.has_child(c4::to_csubstr(name)))
		{
			auto component = node[c4::to_csubstr(name)];
			ComponentSer<TComponent>::Deserialize(registry, component, entity);
		}
	}
public:
	static entt::entity DeserializeEntity(ryml::NodeRef& node, entt::registry& registry, entt::entity entity);
};