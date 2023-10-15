#pragma once
#include <optional>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <ryml/ryml.hpp>

using namespace glm;

struct PhysicsBody {
	vec2 size;
	bool isStatic;

	b2Body* body;
	b2PolygonShape shape;
	b2FixtureDef fixture;
	b2BodyDef bodyDef;

	PhysicsBody(vec2 size = vec2(1.f, 1.f), bool isStatic = false) {
		this->size = size;
		this->isStatic = isStatic;

		this->body = nullptr;
	}
	PhysicsBody(ryml::NodeRef node)
	{
		size = { 1.f,1.f };
		this->isStatic = false;
		if (node.is_map() && node.has_child("size")) {
			node["size"][0] >> size.x;
			node["size"][1] >> size.y;
		}
		if (node.is_map() && node.has_child("isStatic")) {
			isStatic = true;
		}

		this->body = nullptr;
	}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["size"] |= ryml::SEQ;
		node["size"].append_child() << size.x;
		node["size"].append_child() << size.y;
		if (isStatic) {
			node["isStatic"] << isStatic;
		}
	}
};