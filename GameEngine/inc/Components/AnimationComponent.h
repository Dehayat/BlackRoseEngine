#pragma once
#include <string>

#include <ryml/ryml.hpp>
#include <SDL2/SDL_rect.h>

struct AnimationComponent {
	std::string animation;

	AnimationComponent(std::string animaiton) {
		this->animation = animaiton;
	}

	AnimationComponent(ryml::NodeRef node)
	{
		this->animation= "";
		if (node.is_map()) {
			if (node.has_child("animation")) {
				node["animation"] >> this->animation;
			}
		}
	}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["animtion"] << animation;
	}
};