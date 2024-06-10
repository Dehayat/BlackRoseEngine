#pragma once
#include <optional>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <ryml/ryml.hpp>

using namespace glm;

struct PhysicsBodyComponent {
	vec2 size;
	bool isStatic;
	bool isSensor;
	bool useGravity;

	vec2 globalSize;
	b2Body* body;
	b2PolygonShape shape;
	b2FixtureDef fixture;
	b2BodyDef bodyDef;

	PhysicsBodyComponent(vec2 size = vec2(1.f, 1.f), bool isStatic = false, bool isSensor = false, bool useGravity = true) {
		this->size = size;
		this->isStatic = isStatic;
		this->isSensor = isSensor;
		this->useGravity = useGravity;

		this->body = nullptr;
		globalSize = vec2();
	}
	PhysicsBodyComponent(ryml::NodeRef node)
	{
		size = { 1.f,1.f };
		this->isStatic = false;
		this->isSensor = false;
		this->useGravity = true;
		if (node.is_map() && node.has_child("size")) {
			node["size"][0] >> size.x;
			node["size"][1] >> size.y;
		}
		if (node.has_child("isStatic")) {
			isStatic = true;
		}
		if (node.has_child("isSensor")) {
			isSensor = true;
		}
		if (node.has_child("useGravity")) {
			useGravity = true;
		}

		globalSize = vec2();
		this->body = nullptr;
	}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["size"] |= ryml::SEQ;
		node["size"].append_child() << size.x;
		node["size"].append_child() << size.y;
		node["isStatic"] << isStatic;
		node["isSensor"] << isSensor;
		node["useGravity"] << useGravity;
	}
};