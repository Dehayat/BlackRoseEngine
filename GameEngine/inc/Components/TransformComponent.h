#pragma once
#include <optional>
#include <glm/glm.hpp>
#include <entt/entity/entity.hpp>
#include <ryml/ryml.hpp>
#include "Logger.h"

using namespace glm;
using namespace entt;
struct TransformComponent {
	vec2 position;
	vec2 scale;
	float rotation;
	bool hasParent;
	std::uint64_t parentGUID;

	int level;
	std::optional<entity> parent;
	mat3 matrix;

	TransformComponent(vec2 position = vec2(0, 0), vec2 scale = vec2(1, 1), float rotation = 0, std::optional<entity> parent = std::nullopt) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
		this->parent = parent;

		this->level = 0;
		this->matrix = mat3(0);

		this->hasParent = false;
		this->parentGUID = -1;
	}
	TransformComponent(ryml::NodeRef node) {
		this->position = vec2(0, 0);
		this->scale = vec2(1, 1);
		this->rotation = 0;
		this->parentGUID = 0;

		this->parent = std::nullopt;
		this->matrix = mat3(0);
		this->hasParent = false;
		this->level = 0;

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
		if (parent) {
			node["parent"] << parentGUID;
		}
	}
};