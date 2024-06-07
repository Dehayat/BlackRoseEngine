#pragma once
#include <optional>

#include <glm/glm.hpp>
#include <entt/entity/entity.hpp>
#include <ryml/ryml.hpp>

#include "Entity.h"

#include "Components/Components.h"

#include "Debugging/Logger.h"

using namespace glm;
using namespace entt;

struct TransformComponent :IComponent {
	vec2 position;
	vec2 scale;
	float rotation;
	bool hasParent;
	Guid parentGUID;

	int level;
	std::optional<entity> parent;
	mat3 matrixL2W;
	vec2 globalPosition;
	vec2 globalScale;
	float globalRotation;

	//TransformComponent(const TransformComponent&) = delete;
	//TransformComponent& operator=(const TransformComponent&) = delete;

	TransformComponent(vec2 position = vec2(0, 0), vec2 scale = vec2(1, 1), float rotation = 0, std::optional<entity> parent = std::nullopt) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
		this->parent = parent;

		this->level = 0;
		this->matrixL2W = mat3(0);
		globalPosition = vec2();
		globalScale = vec2();
		globalRotation = 0;
		this->parentGUID = -1;
		this->hasParent = false;
		if (this->parent) {
			this->hasParent = true;
		}
	}
	TransformComponent(ryml::NodeRef& node) {
		this->position = vec2(0, 0);
		this->scale = vec2(1, 1);
		this->rotation = 0;
		this->parentGUID = 0;

		this->parent = std::nullopt;
		this->matrixL2W = mat3(0);
		this->hasParent = false;
		this->level = 0;
		globalPosition = vec2();
		globalScale = vec2();
		globalRotation = 0;

		if (node.has_child("position")) {
			node["position"][0] >> position.x;
			node["position"][1] >> position.y;
		}
		if (node.has_child("scale")) {
			node["scale"][0] >> scale.x;
			node["scale"][1] >> scale.y;
		}
		if (node.has_child("rotation")) {
			node["rotation"] >> rotation;
		}
		if (node.has_child("parent")) {
			hasParent = true;
			node["parent"] >> parentGUID;
		}
	}

	//TransformComponent(TransformComponent&& other) {

	//}

	//TransformComponent& operator=(TransformComponent&& other) {

	//	return *this;
	//}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["position"] |= ryml::SEQ;
		node["position"].append_child() << position.x;
		node["position"].append_child() << position.y;
		node["scale"] |= ryml::SEQ;
		node["scale"].append_child() << scale.x;
		node["scale"].append_child() << scale.y;
		node["rotation"] << rotation;
		if (hasParent) {
			node["parent"] << parentGUID;
		}
	}
};