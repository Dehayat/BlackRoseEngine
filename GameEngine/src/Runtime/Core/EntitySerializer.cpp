#include "EntitySerializer.h"

#include "Components/TransformComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/GUIDComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/ScriptComponent.h"
#include "Components/SendEventsToParentComponent.h"
#include "Components/DisableComponent.h"

entt::entity EntitySerializer::DeserializeEntity(ryml::NodeRef& node, entt::registry& registry, entt::entity entity)
{
	DeserializeComponent<DisableComponent>(registry, "Disabled", entity, node);
	DeserializeComponent<TransformComponent>(registry, "Transform", entity, node);
	DeserializeComponent<PhysicsBodyComponent>(registry, "PhysicsBody", entity, node);
	DeserializeComponent<CameraComponent>(registry, "Camera", entity, node);
	DeserializeComponent<AnimationComponent>(registry, "Animation", entity, node);
	DeserializeComponent<SpriteComponent>(registry, "Sprite", entity, node);
	DeserializeComponent<ScriptComponent>(registry, "Script", entity, node);
	DeserializeComponent<SendEventsToParentComponent>(registry, "SendEventsToParent", entity, node);
	return entity;
}
