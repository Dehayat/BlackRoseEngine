#pragma once

#include <string>
#include <memory>
#include<optional>

#include <SDL2/SDL_rect.h>
#include <ryml/ryml.hpp>
#include <glm/glm.hpp>
using namespace glm;

struct SpriteComponent {
	std::string sprite;
	int layer;
	glm::vec4 color;

	SDL_Rect* sourceRect;

	SpriteComponent(std::string sprite = "block", int layer = 0, vec4 color = vec4(1, 1, 1, 1)) {
		this->sprite = sprite;
		this->layer = layer;
		this->color = color;
		sourceRect = nullptr;
	}
	SpriteComponent(ryml::NodeRef node)
	{
		this->sprite = "";
		this->color = vec4(1, 1, 1, 1);
		this->layer = 0;
		sourceRect = nullptr;
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