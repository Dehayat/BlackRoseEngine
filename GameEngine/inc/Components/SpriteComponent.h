#pragma once
#include <string>
#include <ryml/ryml.hpp>
#include <glm/glm.hpp>
using namespace glm;

struct Sprite {
	std::string sprite;
	int layer;
	glm::vec4 color;
	Sprite(std::string sprite, int layer = 0, vec4 color = vec4(1, 1, 1, 1)) {
		this->sprite = sprite;
		this->layer = layer;
		this->color = color;

	}
	Sprite(ryml::NodeRef node)
	{
		this->sprite = "";
		this->color = vec4(1, 1, 1, 1);
		this->layer = 0;
		if (node.is_map()) {
			if (node.has_child("sprite")) {
				node["sprite"] >> this->sprite;
			}
			if (node.has_child("layer")) {
				node["layer"] >> this->layer;
			}
			if (node.has_child("color")) {
				auto c = vec4();
				node["color"][0] >> c.r;
				node["color"][1] >> c.g;
				node["color"][2] >> c.b;
				node["color"][3] >> c.a;
				this->color = c;
			}
		}
	}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["sprite"] << sprite;
		node["layer"] << layer;
		node["color"] |= ryml::SEQ;
		node["color"].append_child() << color.x;
		node["color"].append_child() << color.y;
		node["color"].append_child() << color.z;
		node["color"].append_child() << color.w;
	}
};